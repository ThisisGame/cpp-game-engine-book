#define GLFW_INCLUDE_NONE


#include "fbxsdk.h"
#include "Common/Common.h"
#include "engine.h"


int InitFbxSDK(const char* pFileName);
/// transform FBxAMatrix to glm::mat4
glm::mat4 FbxMatrixToGlmMat4(FbxAMatrix& fbxAMatrix){
    glm::mat4 glm_matrix;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            glm_matrix[i][j] = fbxAMatrix[i][j];
        }
    }
    return glm_matrix;
}
void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition,FbxMesh* pMesh,FbxCluster* pCluster,FbxAMatrix& pVertexTransformMatrix,FbxTime pTime);
void ParseNodeRecursive(FbxNode* pNode);

FbxManager * mSdkManager;
FbxScene * mScene;
FbxImporter * mImporter;
FbxAnimStack * mCurrentAnimationStack;
FbxTime mCurrentTime;
FbxTime mStart,mStop;//动画片段开始结束时间。

int main(void){
    Debug::Init();

    InitFbxSDK("../data/model/fbx_extra.fbx");

    // 获取所有的动画片段
    FbxArray<FbxString*> mAnimStackNameArray;
    mScene->FillAnimStackNameArray(mAnimStackNameArray);
    // 动画片段个数
//    const int lAnimStackCount = mAnimStackNameArray.GetCount();

    // 选择一个动画片段。
    int pIndex=0;
    mCurrentAnimationStack = mScene->FindMember<FbxAnimStack>(mAnimStackNameArray[pIndex]->Buffer());
    if (mCurrentAnimationStack == NULL)
    {
        DEBUG_LOG_ERROR("No animation stack found,name:{}" , mAnimStackNameArray[pIndex]->Buffer());
        return -1;
    }
    // 设置当前Animation Stack
    mScene->SetCurrentAnimationStack(mCurrentAnimationStack);
    // 获取动画片段的时间范围
    FbxTakeInfo* lCurrentTakeInfo = mScene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
    if (lCurrentTakeInfo)
    {
        mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
        mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
    }
    FbxAMatrix lDummyGlobalPosition;
    ParseNodeRecursive(mScene->GetRootNode());

    DEBUG_LOG_INFO("extra animation success");

    return 0;
}

int InitFbxSDK(const char* pFileName){
    // 初始化FBX SDKManager，并创建一个Scene，用来容纳从FBX中解析的所有对象
    InitializeSdkObjects(mSdkManager, mScene);
    if (!mSdkManager) {
        DEBUG_LOG_ERROR("Failed to create FBX SDK manager.");
        return -1;
    }
    // 创建一个Importer，用来解析FBX文件
    int lFileFormat = -1;
    mImporter = FbxImporter::Create(mSdkManager, "");
    if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(pFileName, lFileFormat)) {
        // 未能识别文件格式
        DEBUG_LOG_ERROR("Unrecognizable file format.");
        return -1;
    }

    // 初始化Importer，设置文件路径
    if (mImporter->Initialize(pFileName, lFileFormat) == false) {
        DEBUG_LOG_ERROR("Call to FbxImporter::Initialize() failed.Error reported: {}", mImporter->GetStatus().GetErrorString());
        return -1;
    }

    // 将FBX文件解析导入到Scene中
    bool lResult = mImporter->Import(mScene);
    if (!lResult) {
        DEBUG_LOG_ERROR("Call to FbxImporter::Import() failed.Error reported: {}", mImporter->GetStatus().GetErrorString());
    }

    // 检查Scene完整性
    FbxStatus status;
    FbxArray<FbxString *> details;
    FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(mScene), &status, &details);
    bool lNotify = (!sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData) && details.GetCount() > 0) ||
                   (mImporter->GetStatus().GetCode() != FbxStatus::eSuccess);
    //输出错误信息
    if (lNotify) {
        DEBUG_LOG_ERROR("\n");
        DEBUG_LOG_ERROR("********************************************************************************\n");
        if (details.GetCount()) {
            DEBUG_LOG_ERROR("Scene integrity verification failed with the following errors:\n");

            for (int i = 0; i < details.GetCount(); i++)
                DEBUG_LOG_ERROR("   %s\n", details[i]->Buffer());

            FbxArrayDelete<FbxString *>(details);
        }

        if (mImporter->GetStatus().GetCode() != FbxStatus::eSuccess) {
            DEBUG_LOG_ERROR("\n");
            DEBUG_LOG_ERROR("WARNING:\n");
            DEBUG_LOG_ERROR("   The importer was able to read the file but with errors.\n");
            DEBUG_LOG_ERROR("   Loaded scene may be incomplete.\n\n");
            DEBUG_LOG_ERROR("   Last error message:'%s'\n", mImporter->GetStatus().GetErrorString());
        }

        DEBUG_LOG_ERROR("********************************************************************************\n");
        DEBUG_LOG_ERROR("\n");
        return -1;
    }

    // 转换坐标系为右手坐标系。
    FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
    FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
    if (SceneAxisSystem != OurAxisSystem) {
        OurAxisSystem.ConvertScene(mScene);
    }

    // 转换单元长度
    FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
    if (SceneSystemUnit.GetScaleFactor() != 1.0) {
        // 例子中用的是厘米，所以这里也要转换
        FbxSystemUnit::cm.ConvertScene(mScene);
    }

    // 转换曲面到三角形
    FbxGeometryConverter lGeomConverter(mSdkManager);
    try {
        lGeomConverter.Triangulate(mScene, /*replace*/true);
    } catch (std::runtime_error) {
        DEBUG_LOG_ERROR("Scene integrity verification failed.\n");
        return -1;
    }
    return 0;
}

void ParseNodeRecursive(FbxNode* pNode){
    auto name=pNode->GetName();
    DEBUG_LOG_INFO("node name: {}", name);
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute)
    {
        // 获取pNode相对于锚点的offset
        const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
        FbxAMatrix lGeometryOffset = FbxAMatrix(lT, lR, lS);

        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            FbxMesh* pMesh = pNode->GetMesh();
            // 获取蒙皮数量，一般来说一个Mesh对应一个蒙皮修改器。
//            int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
            FbxMesh* lMesh = pNode->GetMesh();
            const int lVertexCount = lMesh->GetControlPointsCount();//实际顶点数量

            // 实际顶点权重数据
            Engine::VertexRelateBoneInfo* vertex_relate_bone_infos_=new Engine::VertexRelateBoneInfo[lVertexCount];
            // 获取蒙皮修改器
            int lSkinIndex=0;
            FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
            // 获取蒙皮修改器上的顶点组数量，一般来说就是骨骼数量，绑定的时候，一般是以一个骨骼作为一个顶点组。
            int lClusterCount = lSkinDeformer->GetClusterCount();
            // 遍历骨骼
            for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex) {
                // 获取骨骼的顶点组
                FbxCluster *lCluster = lSkinDeformer->GetCluster(lClusterIndex);

                // 获取这个顶点组影响的顶点索引数量
                int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
                for (int k = 0; k < lVertexIndexCount; ++k) {
                    //拿到顶点索引
                    int lIndex = lCluster->GetControlPointIndices()[k];
                    //拿到这个簇中对这个顶点的权重
                    double lWeight = lCluster->GetControlPointWeights()[k];

                    vertex_relate_bone_infos_[lIndex].Push(lClusterIndex,(int)(lWeight*100));
                }
            }

            //上面记录了所有实际顶点的权重，下面要设置到逻辑顶点上。
            const FbxVector4 * lControlPoints = pMesh->GetControlPoints();

            // 是否有UV数据？
            bool mHasUV = pMesh->GetElementUVCount() > 0;
            bool mAllByControlPoint = true;
            FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
            if (mHasUV) {
                lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
                if (lUVMappingMode == FbxGeometryElement::eNone) {
                    mHasUV = false;
                }
                if (mHasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint) {
                    mAllByControlPoint = false;
                }
            }
            // 获取Mesh多边形个数，对游戏来说就是三角形面数。
            const int lPolygonCount = pMesh->GetPolygonCount();
            int lPolygonVertexCount = mAllByControlPoint?pMesh->GetControlPointsCount():lPolygonCount * 3;

            Engine::WeightFile weightFile(lPolygonVertexCount);

            // 遍历所有三角面，遍历每个面的三个顶点，解析顶点坐标、UV坐标数据。
            for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
                // 三角面，3个顶点
                for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex) {
                    // 传入面索引，以及当前面的第几个顶点，获取顶点索引。
                    const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
                    if (lControlPointIndex >= 0) {
                        Engine::VertexRelateBoneInfo vertex_relate_bone_info=vertex_relate_bone_infos_[lControlPointIndex];
                        for (int i = 0; i < 4; ++i) {
                            char bone_index=vertex_relate_bone_info.bone_index_[i];
                            char weight=vertex_relate_bone_info.bone_weight_[i];
                            weightFile.Push(lPolygonIndex*3+lVerticeIndex,bone_index,weight);
                        }
                    }
                }
            }
            weightFile.Write(fmt::format("../data/model/fbx_extra_{}.weight", name).c_str());
        }
    }
    // 遍历子节点，递归
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
        ParseNodeRecursive(pNode->GetChild(lChildIndex));
    }
}
