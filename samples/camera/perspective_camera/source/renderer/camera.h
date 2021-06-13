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

    /// 设置相机位置 朝向
    /// \param cameraPosition 相机位置
    /// \param cameraFowrad 相机朝前方向
    /// \param cameraUp 相机朝上方向
    void SetView(const glm::vec3& cameraPosition,const glm::vec3& cameraFowrad,const glm::vec3& cameraUp);
    /// 设置相机视野
    /// \param fovDegrees   相机视野 可视角度
    /// \param aspectRatio  宽高比
    /// \param nearClip 近裁剪面
    /// \param farClip  远裁剪面
    void SetProjection(float fovDegrees,float aspectRatio,float nearClip,float farClip);


    glm::mat4& view_mat4(){return view_mat4_;}
    glm::mat4& projection_mat4(){return projection_mat4_;}

    /// 设置清屏颜色
    /// \param r
    /// \param g
    /// \param b
    /// \param a
    void set_clear_color(float r,float g,float b,float a){clear_color_=glm::vec4(r,g,b,a);}

    /// 设置刷帧清屏内容种类
    /// \param clear_flag
    void set_clear_flag(unsigned int clear_flag){clear_flag_=clear_flag;}



    void Clear();

private:

    glm::mat4 view_mat4_;//指定相机坐标和朝向
    glm::mat4 projection_mat4_;//指定相机范围

    glm::vec4 clear_color_;//清屏颜色
    unsigned int clear_flag_;//刷新数据标志
};


#endif //UNTITLED_CAMERA_H
