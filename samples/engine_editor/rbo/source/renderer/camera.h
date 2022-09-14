//
// Created by captain on 2021/5/25.
//

#ifndef UNTITLED_CAMERA_H
#define UNTITLED_CAMERA_H

#include <iostream>
#include <vector>
#include <functional>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include "component/component.h"

/// 清除缓存标记
enum BufferClearFlag{
    CLEAR_COLOR_BUFFER=GL_COLOR_BUFFER_BIT,
    CLEAR_DEPTH_BUFFER=GL_DEPTH_BUFFER_BIT,
    CLEAR_STENCIL_BUFFER=GL_STENCIL_BUFFER_BIT,
};

class RenderTarget;
class Camera: public Component {
public:
    Camera();
    ~Camera();

    /// 设置相机位置 朝向
    /// \param cameraForward 相机朝前方向
    /// \param cameraUp 相机朝上方向
    void SetView(const glm::vec3& cameraForward, const glm::vec3& cameraUp);
    /// 设置相机视野
    /// \param fovDegrees   相机视野 可视角度
    /// \param aspectRatio  宽高比
    /// \param nearClip 近裁剪面
    /// \param farClip  远裁剪面
    void SetPerspective(float fovDegrees, float aspectRatio, float nearClip, float farClip);


    glm::mat4& view_mat4(){return view_mat4_;}
    glm::mat4& projection_mat4(){return projection_mat4_;}

    /// 设置正交相机矩阵
    /// \param left
    /// \param right
    /// \param bottom
    /// \param top
    /// \param z_near
    /// \param z_far
    void SetOrthographic(float left,float right,float bottom,float top,float z_near,float z_far);

    /// 设置清屏颜色
    /// \param r
    /// \param g
    /// \param b
    /// \param a
    void set_clear_color(float r,float g,float b,float a){clear_color_=glm::vec4(r,g,b,a);}

    /// 设置刷帧清屏内容种类
    /// \param clear_flag
    void set_clear_flag(unsigned int clear_flag){clear_flag_=clear_flag;}

    /// 刷帧清屏
    void Clear();

    unsigned char depth(){return depth_;}

    /// 设置 depth，触发相机排序
    /// \param depth 相机深度，多相机按从后往前排序，按depth从小往大遍历，depth越大的越上层。
    void set_depth(unsigned char depth);

    unsigned char culling_mask(){return culling_mask_;}
    void set_culling_mask(unsigned char culling_mask){culling_mask_=culling_mask;}

    /// 相机用于
    enum CameraUseFor{
        UI,
        SCENE
    };
    CameraUseFor camera_use_for(){return camera_use_for_;}

    /// 检查render_target_是否设置，是则使用FBO，渲染到RenderTexture/RenderBuffer。
    void CheckRenderTarget();

    void CheckCancelRenderTarget();

    /// 设置渲染目标RenderTarget
    /// \param render_target
    void set_render_target(RenderTarget* render_target);

    /// 清空渲染目标RenderTarget
    void clear_render_target();

    /// 设置当前相机对应的ViewPort大小
    /// \param view_port_width
    /// \param view_port_height
    void set_view_port_size(unsigned short view_port_width,unsigned short view_port_height);

    void UpdateViewPortSize();

    void SetRenderRectInWindow();

    /// 限定在窗口的渲染区域
    /// \param x
    /// \param y
    /// \param width
    /// \param height
    void set_render_rect_in_window(int x,int y,int width,int height){
        render_rect_in_window_={x,y,width,height};
    }
protected:
    glm::mat4 view_mat4_;//指定相机坐标和朝向
    glm::mat4 projection_mat4_;//指定相机范围

    glm::vec4 clear_color_;//清屏颜色
    unsigned int clear_flag_;//刷新数据标志

    unsigned char depth_;//排序深度

    unsigned char culling_mask_;//控制渲染哪些Layer的物体

    CameraUseFor camera_use_for_=CameraUseFor::SCENE;

    RenderTarget* render_target_;//渲染目标 可选RenderTexture/RenderBuffer

    unsigned short view_port_width_;
    unsigned short view_port_height_;

    glm::ivec4 render_rect_in_window_;//限定在窗口的渲染区域
public:
    /// 遍历所有Camera
    /// \param func
    static void Foreach(std::function<void()> func);

    /// 遍历all_camera_时，轮到的那个Camera。
    /// \return
    static Camera* current_camera(){return current_camera_;}

    /// 按 depth_ 排序
    static void Sort();

private:
    static std::vector<Camera*> all_camera_;//所有Camera，每一帧都遍历Camera，设置current_camera_。
    static Camera* current_camera_;//当前用于渲染的Camera，就是MeshRenderer在计算MVP的时候，用这个Camera的View Projection矩阵计算MVP。

RTTR_ENABLE();
};


#endif //UNTITLED_CAMERA_H
