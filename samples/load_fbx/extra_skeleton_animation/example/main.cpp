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

/// 从Mesh节点获取蒙皮修改器，获取所有Cluster。遍历帧数，获取每一帧每个Cluster的移动旋转矩阵数据。
/// \param pNode Mesh节点
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

        DEBUG_LOG_INFO(" node translation: ({},{},{},{})", lT[0], lT[1], lT[2], lT[3]);
        DEBUG_LOG_INFO(" node rotation: ({},{},{},{})", lR[0], lR[1], lR[2], lR[3]);
        DEBUG_LOG_INFO(" node scaling: ({},{},{},{})", lS[0], lS[1], lS[2], lS[3]);

        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            FbxMesh* pMesh = pNode->GetMesh();
            // 获取蒙皮数量，一般来说一个Mesh对应一个蒙皮修改器。
//            int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
            Engine::Animation animation;
            animation.name_=mCurrentAnimationStack->GetName();

            FbxTime::EMode lTimeMode = mScene->GetGlobalSettings().GetTimeMode();
            animation.frame_per_second_=fbxsdk::FbxTime::GetFrameRate(lTimeMode);

            // 获取蒙皮修改器
            int lSkinIndex=0;
            FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
            // 获取蒙皮修改器上的顶点组数量，一般来说就是骨骼数量，绑定的时候，一般是以一个骨骼作为一个顶点组。
            int lClusterCount = lSkinDeformer->GetClusterCount();
            // 遍历骨骼
            for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex) {
                // 获取骨骼的顶点组
                FbxCluster *lCluster = lSkinDeformer->GetCluster(lClusterIndex);
                animation.bone_name_vec_.push_back(lCluster->GetName());
            }

            // 每一帧的时间
            FbxTime mFrameTime;
            mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());
            for(FbxTime pTime=mStart;pTime<mStop;pTime+=mFrameTime){
                // 首先获取当前节点的全局坐标
                FbxAMatrix lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
                FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;//相乘获得pNode在当前时间相对原点的坐标。

                std::vector<glm::mat4> one_frame_bone_matrix_vec;//一帧的所有骨骼变换矩阵
                for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex) {
                    // 获取骨骼的顶点组
                    FbxCluster *lCluster = lSkinDeformer->GetCluster(lClusterIndex);
                    // 计算这个骨骼的形变，前面pNode是指计算到Mesh节点的形变，而这是是计算骨骼节点，后面会作用到顶点。
                    FbxAMatrix lVertexTransformMatrix;
                    ComputeClusterDeformation(lGlobalOffPosition, pMesh, lCluster, lVertexTransformMatrix, pTime);

                    glm::mat4 bone_matrix= FbxMatrixToGlmMat4(lVertexTransformMatrix);
                    one_frame_bone_matrix_vec.push_back(bone_matrix);
                }
                animation.frame_bones_matrix_vec_.push_back(one_frame_bone_matrix_vec);
            }//lClusterCount

            animation.frame_count_=animation.frame_bones_matrix_vec_.size();

            animation.Write(fmt::format("../data/animation/fbx_extra_{}.skeleton_anim", animation.name_).c_str());
        }
    }
    // 遍历子节点，递归
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
        ParseNodeRecursive(pNode->GetChild(lChildIndex));
    }
}

/// Compute the transform matrix that the cluster will transform the vertex.
/// \param pGlobalPosition 节点在当前帧的全局坐标 几何矩阵
/// \param pMesh Mesh
/// \param pCluster 顶点组，一般是一个 Bone一个顶点组，就当成是一个Bone吧。
/// \param pVertexTransformMatrix 存储顶点形变矩阵
/// \param pTime 当前帧时间
/// \param pPose 当前选择的Pose，应该是指动画片段
void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition,FbxMesh* pMesh,FbxCluster* pCluster,FbxAMatrix& pVertexTransformMatrix,FbxTime pTime)
{
    FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

    if (lClusterMode == FbxCluster::eNormalize)
    {
        FbxAMatrix lReferenceGlobalInitPosition;
        FbxAMatrix lAssociateGlobalInitPosition;
        FbxAMatrix lAssociateGlobalCurrentPosition;
        FbxAMatrix lClusterGlobalInitPosition;
        FbxAMatrix lClusterGlobalCurrentPosition;

        FbxAMatrix lAssociateGeometry;
        FbxAMatrix lClusterGeometry;

        FbxAMatrix lClusterRelativeInitPosition;
        FbxAMatrix lClusterRelativeCurrentPositionInverse;
        pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
        // Multiply lReferenceGlobalInitPosition by Geometric Transformation
        const FbxVector4 lT = pMesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lR = pMesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 lS = pMesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
        FbxAMatrix lReferenceGeometry = FbxAMatrix(lT, lR, lS);

        lReferenceGlobalInitPosition *= lReferenceGeometry;

        // Get the link initial global position and the link current global position.
        pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);

        lClusterGlobalCurrentPosition = pCluster->GetLink()->EvaluateGlobalTransform(pTime);

        // Compute the initial position of the link relative to the reference.
        lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

        // Compute the current position of the link relative to the reference.
        lClusterRelativeCurrentPositionInverse = pGlobalPosition.Inverse() * lClusterGlobalCurrentPosition;

        // Compute the shift of the link relative to the reference.
        pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
    }
}
