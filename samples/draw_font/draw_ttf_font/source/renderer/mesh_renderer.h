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
    ~MeshRenderer();

    void SetMaterial(Material* material);//设置Material
    Material* material(){return material_;};;

    void Render();//渲染
private:
    Material* material_;

    unsigned int vertex_buffer_object;//顶点缓冲区对象
    unsigned int element_buffer_object;//索引缓冲区对象
};


#endif //UNTITLED_MESH_RENDERER_H
