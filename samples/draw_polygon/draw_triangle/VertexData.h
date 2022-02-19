//
// Created by captain on 2021/3/25.
// 顶点坐标 顶点颜色
//

#ifndef UNTITLED_VERTEXDATA_H
#define UNTITLED_VERTEXDATA_H

#include <glm/glm.hpp>

static const glm::vec3 kPositions[3] = {
        glm::vec3{ -1.0f, -1.0f,0.0f},
        glm::vec3{  1.0f, -1.0f,0.0f},
        glm::vec3{   0.f,  1.0f,0.0f}
};

static const glm::vec4 kColors[3] = {
        glm::vec4{ 1.f, 0.f, 0.f ,1.f},
        glm::vec4{ 0.f, 1.f, 0.f ,1.f},
        glm::vec4{ 0.f, 0.f, 1.f ,1.f}
};

#endif //UNTITLED_VERTEXDATA_H
