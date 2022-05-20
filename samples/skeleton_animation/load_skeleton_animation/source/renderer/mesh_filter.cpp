//
// Created by captain on 2021/4/25.
//


#include "mesh_filter.h"
#include <fstream>
#include <rttr/registration>
#include "utils/application.h"

using std::ifstream;
using std::ios;

using namespace rttr;
RTTR_REGISTRATION
{
    registration::class_<MeshFilter>("MeshFilter")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

MeshFilter::MeshFilter()
    :Component(),mesh_(nullptr) {

}

void MeshFilter::LoadMesh(string mesh_file_path) {
    //读取 Mesh文件头
    ifstream input_file_stream(Application::data_path()+mesh_file_path,ios::in | ios::binary);
    MeshFileHead mesh_file_head;
    input_file_stream.read((char*)&mesh_file_head,sizeof(mesh_file_head));
    //读取顶点数据
    unsigned char* vertex_data =(unsigned char*)malloc(mesh_file_head.vertex_num_*sizeof(Vertex));
    input_file_stream.read((char*)vertex_data,mesh_file_head.vertex_num_*sizeof(Vertex));
    //读取顶点索引数据
    unsigned short* vertex_index_data=(unsigned short*)malloc(mesh_file_head.vertex_index_num_*sizeof(unsigned short));
    input_file_stream.read((char*)vertex_index_data,mesh_file_head.vertex_index_num_*sizeof(unsigned short));
    input_file_stream.close();

    mesh_=new Mesh();
    mesh_->vertex_num_=mesh_file_head.vertex_num_;
    mesh_->vertex_index_num_=mesh_file_head.vertex_index_num_;
    mesh_->vertex_data_=(Vertex*)vertex_data;
    mesh_->vertex_index_data_=vertex_index_data;
}



MeshFilter::~MeshFilter() {
    if(mesh_!=nullptr) {
        delete mesh_;
        mesh_=nullptr;
    }
}

void MeshFilter::CreateMesh(std::vector<Vertex> &vertex_data, std::vector<unsigned short> &vertex_index_data) {
    if(mesh_!= nullptr){
        delete mesh_;
        mesh_=nullptr;
    }
    mesh_=new Mesh();
    mesh_->vertex_num_=vertex_data.size();
    mesh_->vertex_index_num_=vertex_index_data.size();

    unsigned short vertex_data_size= mesh_->vertex_num_ * sizeof(Vertex);
    mesh_->vertex_data_= static_cast<Vertex *>(malloc(vertex_data_size));
    memcpy(mesh_->vertex_data_, &vertex_data[0], vertex_data_size);

    unsigned short vertex_index_data_size=mesh_->vertex_num_ * sizeof(Vertex);
    mesh_->vertex_index_data_= static_cast<unsigned short *>(malloc(vertex_index_data_size));
    memcpy(mesh_->vertex_index_data_,&vertex_index_data[0],vertex_index_data_size);
}

