//
// Created by captain on 2021/4/25.
//

#ifndef UNTITLED_MESH_FILTER_H
#define UNTITLED_MESH_FILTER_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "component/component.h"

using std::string;


class MeshFilter:public Component{
public:
    MeshFilter();
    ~MeshFilter();
public:
    //顶点
    struct Vertex{
        glm::vec3 pos_;
        glm::vec4 color_;
        glm::vec2 uv_;
    };

    //Mesh文件头
    struct MeshFileHead{
        char type_[4];
        unsigned short vertex_num_;//顶点个数
        unsigned short vertex_index_num_;//索引个数
    };

    //Mesh数据
    struct Mesh{
        unsigned short vertex_num_;//顶点个数
        unsigned short vertex_index_num_;//索引个数
        Vertex* vertex_data_;//顶点数据
        unsigned short* vertex_index_data_;//顶点索引数据
        ~Mesh(){
            if(vertex_data_!= nullptr){
                delete[] vertex_data_;
                vertex_data_ = nullptr;
            }
            if(vertex_index_data_!= nullptr){
                delete[] vertex_index_data_;
                vertex_index_data_ = nullptr;
            }
        }
    };

    /// 加载Mesh文件
    /// \param mesh_file_path
    void LoadMesh(string mesh_file_path);
    /// 创建 Mesh
    /// \param vertex_data 顶点数据
    /// \param vertex_index_data 索引数据
    void CreateMesh(std::vector<Vertex>& vertex_data,std::vector<unsigned short>& vertex_index_data);

    /// 获取Mesh对象指针
    Mesh* mesh(){return mesh_;};

    /// 获取顶点关联骨骼索引数组，长度为顶点个数
    unsigned char* vertex_relate_bone_index(){return vertex_relate_bone_index_;};

    void set_vertex_relate_bone_index(unsigned char* vertex_relate_bone_index){
        vertex_relate_bone_index_ = vertex_relate_bone_index;
    }

    /// 获取蒙皮Mesh对象指针
    Mesh* skinned_mesh(){return skinned_mesh_;};
    void set_skinned_mesh(Mesh* skinned_mesh){skinned_mesh_ = skinned_mesh;};
private:
    Mesh* mesh_;//Mesh对象

    unsigned char* vertex_relate_bone_index_;//顶点关联骨骼索引，长度为顶点个数
    Mesh* skinned_mesh_;//蒙皮Mesh对象
};


#endif //UNTITLED_MESH_FILTER_H
