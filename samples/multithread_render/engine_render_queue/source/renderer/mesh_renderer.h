//
// Created by captainchen on 2021/5/11.
//

#ifndef UNTITLED_MESH_RENDERER_H
#define UNTITLED_MESH_RENDERER_H

#include <memory>
#include <glm/glm.hpp>
#include "component/component.h"

class Material;
class MeshFilter;
class Texture2D;
class MeshRenderer:public Component{
public:
    MeshRenderer();
    virtual ~MeshRenderer();

    void SetMaterial(Material* material);//设置Material
    Material* material(){return material_;}

    void Render();//渲染
private:
    Material* material_;

    unsigned int vertex_buffer_object_handle_=0;//顶点缓冲区对象句柄
    unsigned int vertex_array_object_handle_=0;//顶点数组对象句柄
};


#endif //UNTITLED_MESH_RENDERER_H
