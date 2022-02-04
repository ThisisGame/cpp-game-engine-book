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
void ParseMesh(const FbxMesh* pMesh,FbxVector4* skinnedMeshControlPoints);
void ParseAnimStack(int pIndex);

void DrawNodeRecursive(FbxNode* pNode,FbxTime& pTime,FbxAMatrix& pParentGlobalPosition);
void DrawMesh();

const char* mFileName="../data/model/fbx_extra.fbx";

FbxManager * mSdkManager;
FbxScene * mScene;
FbxImporter * mImporter;
FbxTime mCurrentTime;
FbxTime mStart,mStop;//动画片段开始结束时间。

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

    // 递归解析节点,保存模型到.Mesh文件。
//    ParseNode(mScene->GetRootNode());


    // 每一帧的时间
    FbxTime mFrameTime;
    mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());

    // 获取所有的动画片段
    FbxArray<FbxString*> mAnimStackNameArray;
    mScene->FillAnimStackNameArray(mAnimStackNameArray);
    // 动画片段个数
    const int lAnimStackCount = mAnimStackNameArray.GetCount();

    // 选择一个动画片段。
    int pIndex=0;
    FbxAnimStack * lCurrentAnimationStack = mScene->FindMember<FbxAnimStack>(mAnimStackNameArray[pIndex]->Buffer());
    if (lCurrentAnimationStack == NULL)
    {
        DEBUG_LOG_ERROR("No animation stack found,name:{}" , mAnimStackNameArray[pIndex]->Buffer());
        return -1;
    }
    // 设置当前Animation Stack
    mScene->SetCurrentAnimationStack(lCurrentAnimationStack);
    // 获取动画片段的时间范围
    FbxTakeInfo* lCurrentTakeInfo = mScene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
    if (lCurrentTakeInfo)
    {
        mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
        mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
    }
    // 将当前时间设置为动画片段的开始时间
    mCurrentTime=mStart+mFrameTime*20;

    FbxAMatrix lDummyGlobalPosition;
    DrawNodeRecursive(mScene->GetRootNode(),mCurrentTime,lDummyGlobalPosition);

    DEBUG_LOG_INFO("extra mesh success");

    return 0;
}

// Scale all the elements of a matrix.
void MatrixScale(FbxAMatrix& pMatrix, double pValue)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pMatrix[i][j] *= pValue;
        }
    }
}

// Sum two matrices element by element.
void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pDstMatrix[i][j] += pSrcMatrix[i][j];
        }
    }
}

/// Compute the transform matrix that the cluster will transform the vertex.
/// \param pGlobalPosition 节点在当前帧的全局坐标 几何矩阵
/// \param pMesh Mesh
/// \param pCluster 簇，一般是一个 Bone一个簇，就当成是一个Bone吧。
/// \param pVertexTransformMatrix 存储顶点形变矩阵
/// \param pTime 当前帧时间
/// \param pPose 当前选择的Pose，应该是指动画片段
void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition,
                               FbxMesh* pMesh,
                               FbxCluster* pCluster,
                               FbxAMatrix& pVertexTransformMatrix,
                               FbxTime pTime)
{
    // 链接模式。链接模式设置链接如何影响控制点的位置以及分配给控制点的权重之间的关系。分配给控制点的权重分布在与 FbxGeometry 类实例相关联的一组链接中。
    FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

    if (lClusterMode == FbxCluster::eNormalize)
    {
        //在 eNormalize 模式下，分配给控制点的权重的总和标准化为 1.0。在此模式下设置关联模型无关紧要。链接的影响是链接节点相对于包含控制点的节点的位移的函数。
        FbxAMatrix lReferenceGlobalInitPosition;
        FbxAMatrix lAssociateGlobalInitPosition;
        FbxAMatrix lAssociateGlobalCurrentPosition;
        FbxAMatrix lClusterGlobalInitPosition;
        FbxAMatrix lClusterGlobalCurrentPosition;

        FbxAMatrix lAssociateGeometry;
        FbxAMatrix lClusterGeometry;

        FbxAMatrix lClusterRelativeInitPosition;
        FbxAMatrix lClusterRelativeCurrentPositionInverse;

        // 获取与包含链接的节点关联的矩阵。包含链接的节点，就是骨骼吧
        // 集群的链接节点指定影响集群控制点的节点（FbxNode）。如果节点是动画的，控制点将相应地移动。
        pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);//获取当前骨骼的几何矩阵，全局还是局部？？？
        // Multiply lReferenceGlobalInitPosition by Geometric Transformation
        const FbxVector4 lT = pMesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lR = pMesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 lS = pMesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
        FbxAMatrix lReferenceGeometry = FbxAMatrix(lT, lR, lS);//当前Mesh所在节点的全局几何矩阵，这个是固定的。

        //将当前Mesh所在节点的全局几何矩阵，作用到当前骨骼的几何矩阵，就是计算当前帧最新的顶点位置时，不仅需要考虑到骨骼的影响，也要考虑到Mesh所在节点的全局坐标。
        lReferenceGlobalInitPosition *= lReferenceGeometry;//现在它包含了骨骼的几何矩阵 和 Mesh所在节点的几何矩阵，应该是全局的。

        // Get the link initial global position and the link current global position.
        pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);//骨骼T-Pose全局几何矩阵？

        //骨骼当前帧当前动画片段的全局几何矩阵？pCluster不等于骨骼，pCluster->GetLink()才是骨骼。为什么会有pCluster->GetTransformMatrix，Cluster又不是节点。
        lClusterGlobalCurrentPosition = pCluster->GetLink()->EvaluateGlobalTransform(pTime);

        // Compute the initial position of the link relative to the reference.
        // 骨骼的几何矩阵 和 Mesh所在节点的几何矩阵之和，减去，骨骼T-Pose全局几何矩阵的逆矩阵，得到骨骼相对的几何矩阵。
        lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

        // Compute the current position of the link relative to the reference.
        // 骨骼当前帧的全局几何矩阵  去除 节点在当前帧的全局几何矩阵 = 骨骼相对全局几何矩阵
        lClusterRelativeCurrentPositionInverse = pGlobalPosition.Inverse() * lClusterGlobalCurrentPosition;

        // Compute the shift of the link relative to the reference.
        // 骨骼相对全局几何矩阵 * 骨骼相对几何矩阵 = 顶点位移矩阵
        pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
    }
}

/// Deform the vertex array in classic linear way.传统的蒙皮修改器 计算当前帧的顶点位置
/// \param pGlobalPosition 节点在当前帧的全局坐标 几何矩阵
/// \param pMesh Mesh
/// \param pTime 当前帧时间
/// \param pVertexArray 存储当前帧最新的顶点坐标
/// \param pPose 当前选择的Pose，应该是指动画片段
void ComputeLinearDeformation(FbxAMatrix& pGlobalPosition,
                              FbxMesh* pMesh,
                              FbxTime& pTime,
                              FbxVector4* pVertexArray)
{
    // All the links must have the same link mode.
    // 簇的模式，就是说骨骼关联顶点的方式，一般是eNormalize模式。
    // 在 eNormalize 模式下，分配给控制点的权重之和标准化为 1.0。在这种模式下设置关联模型不是相关的。
    // 链接的影响是位移的函数链接节点相对于包含控制点的节点。
    FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
    // 获取实际顶点数量
    int lVertexCount = pMesh->GetControlPointsCount();
    // 存储簇的形变，就是在这一帧，每个顶点受到每个骨骼影响*权重，然后将多个骨骼这个数据求和。
    FbxAMatrix* lClusterDeformation = new FbxAMatrix[lVertexCount];
    memset(lClusterDeformation, 0, lVertexCount * (unsigned int)sizeof(FbxAMatrix));
    // 存储一个骨骼对所有顶点的权重。
    double* lClusterWeight = new double[lVertexCount];
    memset(lClusterWeight, 0, lVertexCount * sizeof(double));

    // For all skins and all clusters, accumulate their deformation and weight
    // on each vertices and store them in lClusterDeformation and lClusterWeight.
    // 对于所有皮肤和所有簇，在每个顶点上累加它们的变形和权重，并将它们存储在Cluster Deformation和lClusterWeight中。

    // 获取蒙皮数量，一般来说一个Mesh对应一个蒙皮修改器。
    int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
    for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
    {
        // 获取蒙皮修改器
        FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
        // 获取蒙皮修改器上的簇数量，一般来说就是骨骼数量，绑定的时候，一般是以一个骨骼作为一个簇。
        int lClusterCount = lSkinDeformer->GetClusterCount();
        // 遍历骨骼
        for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
        {
            // 获取骨骼的簇
            FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
            // 计算这个骨骼的形变，前面pNode是指计算到Mesh节点的形变，而这是是计算骨骼节点，后面会作用到顶点。
            FbxAMatrix lVertexTransformMatrix;
            ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime);

            // 这里可以获取到骨骼的矩阵，以及骨骼影响的顶点以及权重。
            // 获取这个簇影响的顶点索引数量
            int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
            for (int k = 0; k < lVertexIndexCount; ++k)
            {
                //拿到顶点索引
                int lIndex = lCluster->GetControlPointIndices()[k];
                //拿到这个簇中对这个顶点的权重
                double lWeight = lCluster->GetControlPointWeights()[k];

                if (lWeight == 0.0)
                {
                    continue;
                }

                // Compute the influence of the link on the vertex.
                FbxAMatrix lInfluence = lVertexTransformMatrix;
                MatrixScale(lInfluence, lWeight);// 将骨骼的几何矩阵乘以权重。

                // Add to the sum of the deformations on the vertex.
                MatrixAdd(lClusterDeformation[lIndex], lInfluence);// 将每个骨骼的影响求和

                // Add to the sum of weights to either normalize or complete the vertex.
                lClusterWeight[lIndex] += lWeight;// 存储每个骨骼的权重之和。

            }//For each vertex
        }//lClusterCount
    }

    //Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
    for (int i = 0; i < lVertexCount; i++)
    {
        FbxVector4 lSrcVertex = pVertexArray[i];
        FbxVector4& lDstVertex = pVertexArray[i];// 计算骨骼的影响后，将顶点存储回。
        double lWeight = lClusterWeight[i];// 获取当前顶点权重。

        // Deform the vertex if there was at least a link with an influence on the vertex,
        if (lWeight != 0.0) // 如果当前顶点有权重，那么计算更新顶点位置。
        {
            lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);// 将多个骨骼的几何矩阵，作用到顶点上。
            // In the normalized link mode, a vertex is always totally influenced by the links.
            lDstVertex /= lWeight;//权重总和可能会大于1,调试法线都是略大于1，这是由于float的精度影响。
            if(lWeight>1.0){
                int a=0;
            }
        }
    }

    delete [] lClusterDeformation;
    delete [] lClusterWeight;
}

/// 解析骨骼蒙皮动画矩阵、顶点权重。
void DrawMesh(FbxNode* pNode, FbxTime& pTime, FbxAMatrix& pGlobalPosition){
    FbxMesh* lMesh = pNode->GetMesh();
    const int lVertexCount = lMesh->GetControlPointsCount();//实际顶点数量
    // No vertex to draw.
    if (lVertexCount == 0) {
        return;
    }
    //是否含有蒙皮
    const bool lHasSkin = lMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
    if(lHasSkin==false){
        return;
    }
    //如果有蒙皮，需要计算更新顶点。创建数组，存储计算更新后的顶点。先复制一份原始顶点坐标，然后再更新过去。
    FbxVector4* lVertexArray = new FbxVector4[lVertexCount];
    memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

    //we need to get the number of clusters 获取蒙皮修改器数量，一般都是一个
    const int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
    //获取簇数量，簇指的是一堆顶点，这堆顶点都受到一个bone影响，簇记录了顶点与顶点权重。一般是一个bone一个簇，记录了受bone影响的顶点以及权重。
    int lClusterCount = 0;
    for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
    {
        lClusterCount += ((FbxSkin *)(lMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
    }
    if (lClusterCount>0)
    {
        //获取蒙皮修改器，获取第0个。一般都只有一个蒙皮修改器。
        FbxSkin * lSkinDeformer = (FbxSkin *)lMesh->GetDeformer(0, FbxDeformer::eSkin);
        FbxSkin::EType lSkinningType = lSkinDeformer->GetSkinningType();
        //eRigid 类型表示刚性蒙皮，这意味着只有一个关节可以影响每个控制点。这个九莲fbx可能是特殊的，一般来说一个节点可被多个骨骼关节控制，手游一般是4个骨骼。
        //eLiner 就是传统的，一个节点可被多个骨骼关节控制。
        //还好这两个都是差不多的逻辑，可以一套流程走。eRigid也可以按照eLiner处理。
        if(lSkinningType == FbxSkin::eRigid || lSkinningType==FbxSkin::eLinear)
        {
            ComputeLinearDeformation(pGlobalPosition, lMesh, pTime, lVertexArray);

            ParseMesh(lMesh,lVertexArray);
        }
    }
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
            DrawMesh(pNode, pTime, lGlobalOffPosition);
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
                ParseMesh(lMesh, nullptr);
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
void ParseMesh(const FbxMesh* pMesh,FbxVector4* skinnedMeshControlPoints){
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
    if(skinnedMeshControlPoints!= nullptr){
        lControlPoints=skinnedMeshControlPoints;
    }

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

