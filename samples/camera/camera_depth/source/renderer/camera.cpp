//
// Created by captain on 2021/5/25.
//

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>

Camera::Camera() {

}


Camera::~Camera() {

}


void Camera::SetView(const glm::vec3& cameraPosition,const glm::vec3 &cameraFowrad,const glm::vec3 &cameraUp) {
    view_mat4_=glm::lookAt(cameraPosition, cameraFowrad, cameraUp);
}

void Camera::SetProjection(float fovDegrees, float aspectRatio, float nearClip, float farClip) {
    projection_mat4_=glm::perspective(glm::radians(fovDegrees),aspectRatio,nearClip,farClip);
}

void Camera::Clear() {
    glClear(clear_flag_);
    glClearColor(clear_color_.r,clear_color_.g,clear_color_.b,clear_color_.a);
}






