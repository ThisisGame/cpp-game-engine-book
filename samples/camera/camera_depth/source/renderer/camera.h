//
// Created by captain on 2021/5/25.
//

#ifndef UNTITLED_CAMERA_H
#define UNTITLED_CAMERA_H

#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    ~Camera();

    void SetView(const glm::vec3& cameraPosition,const glm::vec3& cameraFowrad,const glm::vec3& cameraUp);
    void SetProjection(float fovDegrees,float aspectRatio,float nearClip,float farClip);


    glm::mat4& view_mat4(){return view_mat4_;}
    glm::mat4& projection_mat4(){return projection_mat4_;}

    void set_clear_color(float r,float g,float b,float a){clear_color_=glm::vec4(r,g,b,a);}
    void set_clear_flag(unsigned int clear_flag){clear_flag_=clear_flag;}



    void Clear();

private:

    glm::mat4 view_mat4_;//指定相机坐标和朝向
    glm::mat4 projection_mat4_;//指定相机范围

    glm::vec4 clear_color_;//清屏颜色
    unsigned int clear_flag_;//刷新数据标志
};


#endif //UNTITLED_CAMERA_H
