#define GLFW_INCLUDE_NONE

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast_beauty.hpp>
#include "debug.h"
#include "fbxsdk.h"
#include "Common/Common.h"

namespace Engine{
    //顶点
    struct Vertex{
        glm::vec3 position_;
        glm::vec4 color_;
        glm::vec2 uv_;
    };

    //Mesh文件头
    struct MeshFileHead{
        char type_[4];//文件类型文件头
        char name_[32];//名字
        unsigned short vertex_num_;//顶点个数
        unsigned short vertex_index_num_;//索引个数
    };

    //Mesh文件
    struct MeshFile{
        MeshFileHead head_;
        Vertex *vertex_;
        unsigned short *index_;

        MeshFile(){
            vertex_ = nullptr;
            index_ = nullptr;
        }

        ~MeshFile(){
            if(vertex_ != nullptr){
                delete [] vertex_;
                vertex_ = nullptr;
            }
            if(index_ != nullptr){
                delete [] index_;
                index_ = nullptr;
            }
        }

        // 写入文件
        void Write(const char* filePath){
            std::ofstream file(filePath, std::ios::binary);
            if(file.is_open()){
                file.write(reinterpret_cast<char*>(&head_), sizeof(head_));
                file.write(reinterpret_cast<char*>(vertex_), sizeof(Vertex) * head_.vertex_num_);
                file.write(reinterpret_cast<char*>(index_), sizeof(unsigned short) * head_.vertex_index_num_);
                file.close();
            }
        }
    };
}

void ParseNode(FbxNode * pNode);
void ParseMesh(const FbxMesh* pMesh);
void ParseAnimStack(int pIndex);

void DrawNodeRecursive(FbxNode* pNode,FbxTime& pTime,FbxAMatrix& pParentGlobalPosition);
void DrawMesh();

const char* mFileName="../data/model/fbx_extra.fbx";

FbxManager * mSdkManager;
FbxScene * mScene;
FbxImporter * mImporter;
FbxTime mCurrentTime;

int main(void){
    Debug::Init();

    // 初始化FBX SDKManager，并创建一个Scene，用来容纳从FBX中解析的所有对象
    InitializeSdkObjects(mSdkManager, mScene);
    if (!mSdkManager) {
        DEBUG_LOG_ERROR("Failed to create FBX SDK manager.");
        return -1;
    }
    // 创建一个Importer，用来解析FBX文件
    int lFileFormat = -1;
    mImporter = FbxImporter::Create(mSdkManager, "");
    if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(mFileName, lFileFormat)) {
        // 未能识别文件格式
        DEBUG_LOG_ERROR("Unrecognizable file format.");
        return -1;
    }

    // 初始化Importer，设置文件路径
    if (mImporter->Initialize(mFileName, lFileFormat) == false) {
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

    // 每一帧的时间
    FbxTime mFrameTime;
    mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());

    // 当前时间
    mCurrentTime += mFrameTime;

    // 解析动画
    ParseAnimStack(0);

    // 递归解析节点
    ParseNode(mScene->GetRootNode());

    FbxAMatrix lDummyGlobalPosition;
    DrawNodeRecursive(mScene->GetRootNode(),mCurrentTime,lDummyGlobalPosition);

    DEBUG_LOG_INFO("extra mesh success");

    return 0;
}

/// 解析动画片段
void ParseAnimStack(int pIndex){
    // 获取所有的动画片段
    FbxArray<FbxString*> mAnimStackNameArray;
    mScene->FillAnimStackNameArray(mAnimStackNameArray);
    // 动画片段个数
    const int lAnimStackCount = mAnimStackNameArray.GetCount();

    // 选择一个动画片段。
    FbxAnimStack * lCurrentAnimationStack = mScene->FindMember<FbxAnimStack>(mAnimStackNameArray[pIndex]->Buffer());
    if (lCurrentAnimationStack == NULL)
    {
        DEBUG_LOG_ERROR("No animation stack found,name:{}" , mAnimStackNameArray[pIndex]->Buffer());
        return;
    }
    // 设置当前Animation Stack
    auto mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
    mScene->SetCurrentAnimationStack(lCurrentAnimationStack);
    // 获取动画片段的时间范围
    FbxTime mStart,mStop;
    FbxTakeInfo* lCurrentTakeInfo = mScene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
    if (lCurrentTakeInfo)
    {
        mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
        mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
    }
    // 将当前时间设置为动画片段的开始时间
    mCurrentTime=mStart;

}

/// 解析骨骼蒙皮动画矩阵、顶点权重。
void DrawMesh(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,FbxAMatrix& pGlobalPosition, FbxPose* pPose){

}

void DrawNodeRecursive(FbxNode* pNode,FbxTime& pTime,FbxAMatrix& pParentGlobalPosition){
    // 首先获取当前节点的全局坐标
    FbxAMatrix lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);

    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute)
    {
        // 获取pNode相对于锚点的offset
        const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
        FbxAMatrix lGeometryOffset = FbxAMatrix(lT, lR, lS);
        FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;//相乘获得pNode在当前时间相对原点的坐标。

        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            DrawMesh(pNode, pTime, pAnimLayer, pGlobalPosition, pPose, pShadingMode);
        }
    }
    // 遍历子节点，递归
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
        DrawNodeRecursive(pNode->GetChild(lChildIndex), pTime,lGlobalPosition);
    }
}

/// 递归解析节点
/// @param pNode 节点
void ParseNode(FbxNode * pNode){
    // 获取节点属性
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute){
        // 节点是Mesh
        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
            FbxMesh * lMesh = pNode->GetMesh();
            if (lMesh && !lMesh->GetUserDataPtr()) {
                ParseMesh(lMesh);
            }
        }
    }

    // 递归解析子节点
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        ParseNode(pNode->GetChild(lChildIndex));
    }
}

/// 解析Mesh
/// @param pMesh Mesh 对象
void ParseMesh(const FbxMesh* pMesh){
    FbxNode* lNode = pMesh->GetNode();
    if (!lNode){
        DEBUG_LOG_ERROR("Mesh has no node.");
        return;
    }
    DEBUG_LOG_INFO("Mesh name: {}", lNode->GetName());

    // 获取Mesh多边形个数，对游戏来说就是三角形面数。
    const int lPolygonCount = pMesh->GetPolygonCount();
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

    // 最终顶点个数到底是多少？
    // 如果只有一套UV，即UV映射方式是按实际顶点个数(FbxGeometryElement::eByControlPoint)，那么就是实际顶点个数。
    // 如果有多套UV，那么一个顶点在不同的多边形里，会对应不同的UV坐标，即UV映射方式是按多边形(eByPolygonVertex)，那么顶点个数是多边形数*3.
    int lPolygonVertexCount = mAllByControlPoint?pMesh->GetControlPointsCount():lPolygonCount * 3;
    // 创建数组存放所有顶点坐标。
    float * lVertices = new float[lPolygonVertexCount * 3];
    // 创建数组存放索引数据，数组长度=面数*3.
    unsigned short * lIndices = new unsigned short[lPolygonCount * 3];
    // 获取多套UV名字
    float * lUVs = NULL;
    FbxStringList lUVNames;
    pMesh->GetUVSetNames(lUVNames);
    const char * lUVName = NULL;
    if (mHasUV && lUVNames.GetCount()) {
        // 创建数组存放UV数据
        lUVs = new float[lPolygonVertexCount * 2];
        // 暂时只使用第一套UV。
        lUVName = lUVNames[0];
    }
    // 实际顶点数据。
    const FbxVector4 * lControlPoints = pMesh->GetControlPoints();

    // 遍历所有三角面，遍历每个面的三个顶点，解析顶点坐标、UV坐标数据。
    int lVertexCount = 0;
    for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
        // 三角面，3个顶点
        for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex) {
            // 传入面索引，以及当前面的第几个顶点，获取顶点索引。
            const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
            if (lControlPointIndex >= 0) {
                // 因为设定一个顶点有多套UV，所以每个三角面与其他面相邻的共享的顶点，尽管实际上是同一个点(ControlPoint),因为有不同的UV，所以还是算不同的顶点。
                lIndices[lVertexCount] = static_cast<unsigned short>(lVertexCount);
                // 获取当前顶点索引对应的实际顶点。
                FbxVector4 lCurrentVertex = lControlPoints[lControlPointIndex];
                // 将顶点坐标从FbxVector4转为float数组
                lVertices[lVertexCount * 3] = static_cast<float>(lCurrentVertex[0]);
                lVertices[lVertexCount * 3 + 1] = static_cast<float>(lCurrentVertex[1]);
                lVertices[lVertexCount * 3 + 2] = static_cast<float>(lCurrentVertex[2]);

                if (mHasUV) {
                    // 获取当前顶点在指定UV层的UV坐标，前面说过，一个顶点可能有多套UV。
                    bool lUnmappedUV;
                    FbxVector2 lCurrentUV;
                    pMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV, lUnmappedUV);
                    // 将UV坐标从FbxVector2转为float数组
                    lUVs[lVertexCount * 2] = static_cast<float>(lCurrentUV[0]);
                    lUVs[lVertexCount * 2 + 1] = static_cast<float>(lCurrentUV[1]);
                }
            }
            ++lVertexCount;
        }
    }


    // 创建引擎Mesh文件，从FBX中解析数据填充到里面。
    Engine::MeshFile mesh_file;
    // 构造引擎Mesh结构，设置文件头
    strcpy(mesh_file.head_.type_,"Mesh");
    strcpy(mesh_file.head_.name_,lNode->GetName());
    mesh_file.head_.vertex_num_ = lVertexCount;
    mesh_file.head_.vertex_index_num_ = lVertexCount;
    mesh_file.vertex_ = new Engine::Vertex[mesh_file.head_.vertex_num_];
    mesh_file.index_ = new unsigned short[mesh_file.head_.vertex_index_num_];
    // 填充顶点坐标、color、UV坐标。
    for (int i = 0; i < lVertexCount; ++i) {
        mesh_file.vertex_[i].position_ = glm::vec3(lVertices[i * 3], lVertices[i * 3+1], lVertices[i * 3+2]);
        mesh_file.vertex_[i].color_ = glm::vec4(1.0f);
        mesh_file.vertex_[i].uv_ = glm::vec2(lUVs[i * 2], lUVs[i * 2 + 1]);
    }
    // 填充索引
    mesh_file.index_=lIndices;
    // 写入文件
    mesh_file.Write(fmt::format("../data/model/fbx_extra_{}.mesh", mesh_file.head_.name_).c_str());
}

