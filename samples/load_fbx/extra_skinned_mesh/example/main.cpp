#define GLFW_INCLUDE_NONE


#include <iostream>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

    class Animation{
    public:
        std::string name_;
        unsigned short frame_count_;//帧数
        unsigned short frame_per_second_;//帧率

        // 存储骨骼名字 [bone_name , bone_name , ...]
        std::vector<std::string> bone_name_vec_;

        // 存储骨骼父子关系，数组下标表示第几个Bone [[1],[]]
        std::vector<std::vector<int>> bone_children_vec_;

        // 存储骨骼 T-POSE，数组下标表示第几个Bone ["Bone.Matrix","Bone.001.Matrix"]
        std::vector<glm::mat4> bone_t_pose_vec_;

        // 存储骨骼关键帧数据，外面数组下标表示第几帧，里面数组下表表示第几个Bone [["Bone.Matrix","Bone.001.Matrix"] , ["Bone.Matrix","Bone.001.Matrix"]]
        std::vector<std::vector<glm::mat4>> frame_bones_matrix_vec_;
    public:
        Animation(){
            frame_count_ = 0;
            frame_per_second_ = 0;
        }

        /// 获取骨骼index
        int get_bone_index(std::string bone_name){
            for (int i = 0; i < bone_name_vec_.size(); ++i) {
                if(bone_name_vec_[i]==bone_name){
                    return i;
                }
            }
            return -1;
        }

        // 写入文件
        void Write(std::string filePath){
            // 路径转小写
            std::transform(filePath.begin(), filePath.end(), filePath.begin(), ::tolower);
            // 替换文件名中的空格为_
            std::filesystem::path path(filePath);
            std::string file_name = path.filename().string();
            // 判断文件名中是否存在空格，替换为_
            if(file_name.find(" ") != std::string::npos){
                DEBUG_LOG_ERROR("Animation::Write filePath:{} contains blank,will replace with _", filePath.c_str());
                std::replace(file_name.begin(), file_name.end(), ' ', '_');
                path.replace_filename(file_name);
                filePath = path.string();
            }
            // 判断文件名中是否存在 | ，替换为_
            if(file_name.find("|") != std::string::npos){
                DEBUG_LOG_ERROR("Animation::Write filePath:{} contains |,will replace with _", filePath.c_str());
                std::replace(file_name.begin(), file_name.end(), '|', '_');
                path.replace_filename(file_name);
                filePath = path.string();
            }
            // 创建文件夹
            if (std::filesystem::exists(path)==false){
                DEBUG_LOG_INFO("{} not exist,will create.",filePath);
                if(path.has_filename()==false){
                    DEBUG_LOG_ERROR("{} is not correct file path.",filePath);
                    assert(false);
                }
                auto dir_path = path.parent_path();
                if(std::filesystem::exists(dir_path)==false){
                    std::error_code error_cord;
                    if(std::filesystem::create_directories(dir_path,error_cord)==false){
                        DEBUG_LOG_ERROR("{} not exist,create failed.",filePath);
                        assert(false);
                    }
                }
            }
            // 写入文件
            std::ofstream file(filePath, std::ios::binary);
            if(file.is_open()==false){
                DEBUG_LOG_ERROR("file open failed:{}",filePath);
                assert(false);
                return;
            }
            // 写入文件头 skeleton_anim
            std::string type = "skeleton_anim";
            file.write(type.c_str(), type.size());
            // 写入名字长度
            unsigned short name_len = name_.size();
            file.write(reinterpret_cast<char*>(&name_len), sizeof(unsigned short));
            // 写入名字
            file.write(name_.c_str(), name_.size());
            // 写入帧数
            file.write(reinterpret_cast<char*>(&frame_count_), sizeof(frame_count_));
            // 写入帧率
            file.write(reinterpret_cast<char*>(&frame_per_second_), sizeof(frame_per_second_));
            // 写入骨骼数量
            unsigned short bone_num = bone_name_vec_.size();
            file.write(reinterpret_cast<char*>(&bone_num), sizeof(unsigned short));
            // 写入骨骼名字
            for (int i = 0; i < bone_name_vec_.size(); ++i) {
                // 写入长度
                unsigned short bone_name_len = bone_name_vec_[i].size();
                file.write(reinterpret_cast<char*>(&bone_name_len), sizeof(unsigned short));
                // 写入名字
                file.write(bone_name_vec_[i].c_str(), bone_name_vec_[i].size());
            }
            // 写入骨骼父子关系
            for (int i = 0; i < bone_children_vec_.size(); ++i) {
                unsigned short children_num = bone_children_vec_[i].size();
                file.write(reinterpret_cast<char*>(&children_num), sizeof(unsigned short));
                for (int j = 0; j < bone_children_vec_[i].size(); ++j) {
                    unsigned short child_index = bone_children_vec_[i][j];
                    file.write(reinterpret_cast<char*>(&child_index), sizeof(unsigned short));
                }
            }
            // 写入骨骼 T-POSE
            for (int i = 0; i < bone_t_pose_vec_.size(); ++i) {
                glm::mat4 bone_t_pose = bone_t_pose_vec_[i];
                file.write(reinterpret_cast<char*>(&bone_t_pose), sizeof(glm::mat4));
            }
            // 写入骨骼关键帧数据
            for (int frame_index = 0; frame_index < frame_bones_matrix_vec_.size(); ++frame_index) {
                for (int bone_index = 0; bone_index < frame_bones_matrix_vec_[frame_index].size(); ++bone_index) {
                    glm::mat4 bone_matrix = frame_bones_matrix_vec_[frame_index][bone_index];
                    file.write(reinterpret_cast<char*>(&bone_matrix), sizeof(glm::mat4));
                }
            }
            file.close();
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
    Engine::Animation engine_animation;

    // 收集Bone名字
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        Engine::MeshFile mesh_file;
        aiMesh *mesh = scene->mMeshes[i]; // 获取Mesh
        // 判断是否有骨骼
        if (mesh->HasBones() == false) {
            DEBUG_LOG_ERROR("Mesh {} has no bones", mesh->mName.C_Str());
            return;
        }
        // 遍历Bone，获取名字以及T-Pose矩阵
        for (int bone_index = 0; bone_index < mesh->mNumBones; ++bone_index) {
            aiBone* bone = mesh->mBones[bone_index];
            // 获取Bone名字
            engine_animation.bone_name_vec_.push_back(bone->mName.C_Str());
            // 获取Bone T-Pose
            aiMatrix4x4& bone_matrix = bone->mOffsetMatrix;
            glm::mat4 bone_matrix_glm=glm::mat4(
                    bone_matrix.a1, bone_matrix.b1, bone_matrix.c1, bone_matrix.d1,
                    bone_matrix.a2, bone_matrix.b2, bone_matrix.c2, bone_matrix.d2,
                    bone_matrix.a3, bone_matrix.b3, bone_matrix.c3, bone_matrix.d3,
                    bone_matrix.a4, bone_matrix.b4, bone_matrix.c4, bone_matrix.d4
            );

            bone_matrix_glm=glm::inverse(bone_matrix_glm);
            glm::mat4 rotate_mat4=glm::rotate(glm::mat4(1.0f),glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
            bone_matrix_glm=rotate_mat4*bone_matrix_glm;
            engine_animation.bone_t_pose_vec_.push_back(bone_matrix_glm);
        }
    }

    // 遍历Bone与Node，存储父子关系。
    for (int bone_index = 0; bone_index < engine_animation.bone_name_vec_.size(); ++bone_index) {
        engine_animation.bone_children_vec_.push_back(std::vector<int>());
        std::string bone_name = engine_animation.bone_name_vec_[bone_index];
        aiNode* bone_node = scene->mRootNode->FindNode(bone_name.c_str());
        if (bone_node == nullptr) {
            DEBUG_LOG_ERROR("parent bone {} not found", bone_name);
            continue;
        }
        // 遍历子节点
        for (int child_index = 0; child_index < bone_node->mNumChildren; ++child_index) {
            aiNode* child_node = bone_node->mChildren[child_index];
            // 根据 child_node 名字，找到对应的 bone_index
            int child_bone_index = engine_animation.get_bone_index(child_node->mName.C_Str());
            if (child_bone_index == -1) {
                DEBUG_LOG_ERROR("child bone {} not found", child_node->mName.C_Str());
                continue;
            }
            // 存储父子关系
            engine_animation.bone_children_vec_[bone_index].push_back(child_bone_index);
        }
    }


    // 遍历Animation,一个FBX里面可能有多个Animation。
    for (int i = 0; i < scene->mNumAnimations; ++i) {
        aiAnimation* animation = scene->mAnimations[i];

        char* str="Animation name:{} duration:{} ticksPerSecond:{} channels:{}";
        // 输出 Animation name:Take 001 duration:200 ticksPerSecond:30 channels:43
        // duration 总帧数，ticksPerSecond 每秒帧数
        DEBUG_LOG_INFO(str,animation->mName.C_Str(),animation->mDuration,animation->mTicksPerSecond,animation->mNumChannels);
        engine_animation.name_=animation->mName.C_Str();
        engine_animation.frame_count_=animation->mDuration;
        engine_animation.frame_per_second_=animation->mTicksPerSecond;
        for (int j = 0; j < engine_animation.frame_count_; ++j) {
            engine_animation.frame_bones_matrix_vec_.push_back(std::vector<glm::mat4>());
        }

        // 遍历Channel，一个Channel指一个Node的动画数据，这个Node可能是Bone，也可能是Mesh，也可能是空的父节点。
        // 把FBX拖到Blender里面Hierarchy显示的节点都会有一个Channel对应。
        // 当然Camera、Light这些Scene相关的是没有的。
        // 注意每个Channel的关键帧数量不一致。
        for (int channel_index = 0; channel_index < animation->mNumChannels; ++channel_index) {
            aiNodeAnim* node_anim = animation->mChannels[channel_index];
//            DEBUG_LOG_INFO("Bone {} has {} keyframes", one_bone_anim->mNodeName.C_Str() , one_bone_anim->mNumPositionKeys);
            // 遍历一个Bone的动画数据:位移。
            for (int key_index = 0; key_index < node_anim->mNumPositionKeys; ++key_index) {
                aiVectorKey& key = node_anim->mPositionKeys[key_index];
//                DEBUG_LOG_INFO("Position: Key {}: time {}  value {}",key_index ,key.mTime ,glm::to_string(glm::vec3(key.mValue.x,key.mValue.y,key.mValue.z)));
            }
            // 遍历一个Bone的动画数据:旋转。
            for (int key_index = 0; key_index < node_anim->mNumRotationKeys; ++key_index) {
                aiQuatKey& key = node_anim->mRotationKeys[key_index];
//                DEBUG_LOG_INFO("Rotation: Key {}: time {}  value ({})",key_index ,key.mTime ,glm::to_string(glm::vec4(key.mValue.w,key.mValue.x,key.mValue.y,key.mValue.z)));
            }
            // 遍历一个Bone的动画数据:缩放。
            for (int key_index = 0; key_index < node_anim->mNumScalingKeys; ++key_index) {
                aiVectorKey& key = node_anim->mScalingKeys[key_index];
//                DEBUG_LOG_INFO("Scaling: Key {}: time {}  value {}",key_index ,key.mTime ,glm::to_string(glm::vec3(key.mValue.x,key.mValue.y,key.mValue.z)));
            }
            // 写到这里会发现，Assimp里存储的骨骼动画数据，只存储了关键帧数据，并不像之前在Blender里做的，将每一帧的数据都获取到了。
            // 所以从Assimp中导出的数据，在引擎中读取后，需要自己做插帧计算出当前帧的数据。
            // 插帧算法有很多种，我只做最简单的就是线性插帧，还有贝塞尔曲线插帧方式等，后续再去了解。
        }



        // 计算所有帧的骨骼动画数据
        for (int frame_index = 0; frame_index < animation->mDuration; ++frame_index) {
            // 先填测试数据
            for (int channel_index = 0; channel_index < animation->mNumChannels; ++channel_index) {
                aiNodeAnim *node_anim = animation->mChannels[channel_index];
                auto bone_index=engine_animation.get_bone_index(node_anim->mNodeName.C_Str());
                if(bone_index==-1){
                    DEBUG_LOG_ERROR("node {} is not bone.",node_anim->mNodeName.C_Str());
                    continue;
                }

                engine_animation.frame_bones_matrix_vec_[frame_index].push_back(glm::mat4(1.0f));
            }
        }

        engine_animation.Write(fmt::format("../data/animation/assimp_extra_{}.skeleton_anim", engine_animation.name_).c_str());
    }

    DEBUG_LOG_INFO("extra animation success");
}

int main(void){
    Debug::Init();

    //实例化Assimp Importer
    Assimp::Importer importer;

    // 解析模型文件
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




