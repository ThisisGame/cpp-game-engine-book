#define GLFW_INCLUDE_NONE


#include <iostream>
#include <fstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "debug.h"

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

/// 导出Mesh文件
/// @param scene 导入的场景
void ExtraMesh(const aiScene *scene) {// 遍历Material
    for(unsigned int i = 0; i < scene->mNumMaterials; i++){
        aiMaterial* material = scene->mMaterials[i];
        aiString name;
        material->Get(AI_MATKEY_NAME, name);
        DEBUG_LOG_INFO("Material name: {}", name.C_Str());

        // 遍历 Texture
        for(unsigned int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++){
            aiString path;
            material->GetTexture(aiTextureType_DIFFUSE, j, &path);
            DEBUG_LOG_INFO("Texture path: {}", path.C_Str());

            // 加载EmbeddedTexture
            auto texture = scene->GetEmbeddedTexture(path.C_Str());
            if(texture != nullptr){
                DEBUG_LOG_INFO("EmbeddedTexture width:{} height:{}" , texture->mWidth, texture->mHeight);
                // 将EmbeddedTexture写入文件
                std::ofstream texture_file(texture->mFilename.C_Str(), std::ios::binary);
                if(texture_file.is_open()){
                    texture_file.write(reinterpret_cast<char*>(texture->pcData), texture->mWidth * texture->mHeight * 4);
                    texture_file.close();
                }
            }else{
                DEBUG_LOG_ERROR("Texture {} is not EmbeddedTexture",path.C_Str());
            }
        }
    }

    // 遍历Mesh，一个FBX里面可能有多个Mesh。
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        Engine::MeshFile mesh_file;

        aiMesh* mesh = scene->mMeshes[i]; // 获取Mesh

        DEBUG_LOG_INFO("Mesh {} has {} vertices",mesh->mName.C_Str() ,mesh->mNumVertices);

        // 构造引擎Mesh结构
        strcpy(mesh_file.head_.type_,"Mesh");
        strcpy(mesh_file.head_.name_,mesh->mName.C_Str());
        mesh_file.head_.vertex_num_ = mesh->mNumVertices;
        mesh_file.head_.vertex_index_num_ = mesh->mNumFaces * 3;
        mesh_file.vertex_ = new Engine::Vertex[mesh_file.head_.vertex_num_];
        mesh_file.index_ = new unsigned short[mesh_file.head_.vertex_index_num_];

        // 遍历顶点
        for (int j = 0; j < mesh->mNumVertices; ++j) {
            // 顶点坐标
            aiVector3D& v = mesh->mVertices[j];
//            DEBUG_LOG_INFO("Vertex {} has position {}",j ,glm::to_string(glm::vec3(v.x, v.y, v.z)));

            // 顶点颜色
            glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
            if(mesh->mColors!=nullptr && mesh->mColors[0]!=nullptr){
                aiColor4D& c = mesh->mColors[0][j];//一个顶点有多套顶点颜色，暂时只使用第一套。
                color = glm::vec4(c.r, c.g, c.b, c.a);
//                DEBUG_LOG_INFO("Vertex {} has color {}",j ,glm::to_string(color));
            }

            // 顶点法线
            aiVector3D& n = mesh->mNormals[j];//暂时不使用法线

            // 顶点纹理坐标
            aiVector3D& t = mesh->mTextureCoords[0][j];
//            DEBUG_LOG_INFO("Vertex {} has texture coordinate {}",j ,glm::to_string(glm::vec2(t.x, t.y)));

            mesh_file.vertex_[j].position_ = glm::vec3(v.x, v.y, v.z);
            mesh_file.vertex_[j].color_ = color;
            mesh_file.vertex_[j].uv_ = glm::vec2(t.x, t.y);
        }

        // 获取模型的索引数据
        DEBUG_LOG_INFO("Mesh {} has {} indices",i ,mesh->mNumFaces);
        for (int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace& face = mesh->mFaces[j];
//            DEBUG_LOG_INFO("Face {} has {} indices",j ,face.mNumIndices);
            if(face.mNumIndices!=3){
//                DEBUG_LOG_ERROR("Face {} has {} indices",j ,face.mNumIndices);
                assert(false);
            }
            for (int k = 0; k < face.mNumIndices; ++k) {
//                DEBUG_LOG_INFO("  Index {} is {}",k ,face.mIndices[k]);
                mesh_file.index_[j * 3 + k] = face.mIndices[k];
            }
        }

        // 写入文件
        mesh_file.Write(fmt::format("../data/model/assimp_extra_{}.mesh", mesh_file.head_.name_).c_str());
    }

    DEBUG_LOG_INFO("extra mesh success");
}

/// 顶点关联骨骼及权重,每个顶点最多可以关联4个骨骼。
struct VertexRelateBoneInfo{
    char bone_index_[4];//骨骼索引，一般骨骼少于128个，用char就行。
    char bone_weight_[4];//骨骼权重，权重不会超过100，所以用char类型就可以。

    VertexRelateBoneInfo(){
        for (int i = 0; i < sizeof(bone_index_); ++i) {
            bone_index_[i]=-1;
        }
        for (int i = 0; i < sizeof(bone_weight_); ++i) {
            bone_weight_[i]=-1;
        }
    }

    void Push(char bone_index,char bone_weight){
        for (int i = 0; i < sizeof(bone_index_); ++i) {
            if(bone_index_[i]==-1){
                bone_index_[i]=bone_index;
                if(bone_weight_[i]==-1){
                    bone_weight_[i]=bone_weight;
                    return;
                }
                DEBUG_LOG_ERROR("bone_index_ bone_weight_ not sync");
            }
        }
        DEBUG_LOG_ERROR("too much bone");
    }
};

/// 导出Weight文件
/// @param scene 导入的场景
void ExtraWeight(const aiScene *scene) {// 遍历Material
    // 遍历Mesh，一个FBX里面可能有多个Mesh。
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        Engine::MeshFile mesh_file;

        aiMesh* mesh = scene->mMeshes[i]; // 获取Mesh

        DEBUG_LOG_INFO("Mesh {} has {} vertices",mesh->mName.C_Str() ,mesh->mNumVertices);

        // 判断是否有骨骼
        if(mesh->HasBones()==false){
            DEBUG_LOG_ERROR("Mesh {} has no bones",mesh->mName.C_Str());
            continue;
        }

        VertexRelateBoneInfo* vertex_relate_bone_infos_=new VertexRelateBoneInfo[mesh->mNumVertices];

        // 遍历Bone
        for (int bone_index = 0; bone_index < mesh->mNumBones; ++bone_index) {
            aiBone* bone = mesh->mBones[bone_index];
//            DEBUG_LOG_INFO("Bone {} has {} vertices",bone->mName.C_Str() ,bone->mNumWeights);
            // 遍历顶点
            for (int bone_relate_weight_index = 0; bone_relate_weight_index < bone->mNumWeights; ++bone_relate_weight_index) {
                aiVertexWeight& vertexWeight = bone->mWeights[bone_relate_weight_index];
//                DEBUG_LOG_INFO("  Vertex {} has weight {}", vertexWeight.mVertexId , vertexWeight.mWeight);
                vertex_relate_bone_infos_[vertexWeight.mVertexId].Push(bone_index,vertexWeight.mWeight*100);
            }
        }

        // 写文件
        std::ofstream file(fmt::format("../data/model/assimp_extra_{}.weight", mesh->mName.C_Str()).c_str(), std::ios::binary);
        if(file.is_open()){
            file.write("weight",sizeof("weight"));
            file.write(reinterpret_cast<char*>(vertex_relate_bone_infos_), sizeof(vertex_relate_bone_infos_) * mesh->mNumVertices);
            file.close();
        }
    }


    DEBUG_LOG_INFO("extra weight success");
}

/// 导出骨骼动画
/// @param scene 导入的场景
void ExtraAnimation(const aiScene *scene){
    // 遍历Animation,一个FBX里面可能有多个Animation。
    for (int i = 0; i < scene->mNumAnimations; ++i) {
        aiAnimation* animation = scene->mAnimations[i];

        char* str="Animation name:{} duration:{} ticksPerSecond:{} channels:{}";
        // 输出 Animation name:Take 001 duration:200 ticksPerSecond:30 channels:43
        // duration 总帧数，ticksPerSecond 每秒帧数
        DEBUG_LOG_INFO(str,animation->mName.C_Str(),animation->mDuration,animation->mTicksPerSecond,animation->mNumChannels);

        // 遍历Channel，一个Channel指一个Node的动画数据，这个Node可能是Bone，也可能是Mesh，也可能是空的父节点。
        // 把FBX拖到Blender里面Hierarchy显示的节点都会有一个Channel对应。
        // 当然Camera、Light这些Scene相关的是没有的。
        // 注意每个Channel的关键帧数量不一致。
        for (int channel_index = 0; channel_index < animation->mNumChannels; ++channel_index) {
            aiNodeAnim* one_bone_anim = animation->mChannels[channel_index];
//            DEBUG_LOG_INFO("Bone {} has {} keyframes", one_bone_anim->mNodeName.C_Str() , one_bone_anim->mNumPositionKeys);
            // 遍历一个Bone的动画数据:位移。
            for (int key_index = 0; key_index < one_bone_anim->mNumPositionKeys; ++key_index) {
                aiVectorKey& key = one_bone_anim->mPositionKeys[key_index];
//                DEBUG_LOG_INFO("Position: Key {}: time {}  value {}",key_index ,key.mTime ,glm::to_string(glm::vec3(key.mValue.x,key.mValue.y,key.mValue.z)));
            }
            // 遍历一个Bone的动画数据:旋转。
            for (int key_index = 0; key_index < one_bone_anim->mNumRotationKeys; ++key_index) {
                aiQuatKey& key = one_bone_anim->mRotationKeys[key_index];
//                DEBUG_LOG_INFO("Rotation: Key {}: time {}  value ({})",key_index ,key.mTime ,glm::to_string(glm::vec4(key.mValue.w,key.mValue.x,key.mValue.y,key.mValue.z)));
            }
            // 遍历一个Bone的动画数据:缩放。
            for (int key_index = 0; key_index < one_bone_anim->mNumScalingKeys; ++key_index) {
                aiVectorKey& key = one_bone_anim->mScalingKeys[key_index];
//                DEBUG_LOG_INFO("Scaling: Key {}: time {}  value {}",key_index ,key.mTime ,glm::to_string(glm::vec3(key.mValue.x,key.mValue.y,key.mValue.z)));
            }
            // 写到这里会发现，Assimp里存储的骨骼动画数据，只存储了关键帧数据，并不像之前在Blender里做的，将每一帧的数据都获取到了。
            // 所以从Assimp中导出的数据，在引擎中读取后，需要自己做插帧计算出当前帧的数据。
            // 插帧算法有很多种，我只做最简单的就是线性插帧，还有贝塞尔曲线插帧方式等，后续再去了解。
        }
    }

    DEBUG_LOG_INFO("extra animation success");
}

int main(void){
    Debug::Init();

    //实例化Assimp Importer
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( "../data/model/assimp_extra.fbx",aiProcess_CalcTangentSpace|aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_SortByPType);

    // 读取文件失败
    if (nullptr == scene) {
        DEBUG_LOG_ERROR(importer.GetErrorString());
        return false;
    }
    ExtraMesh(scene);
    ExtraWeight(scene);
    ExtraAnimation(scene);
    return 0;
}




