//
// Created by captain on 2021/7/10.
//

#ifndef UNTITLED_LOGINSCENE_H
#define UNTITLED_LOGINSCENE_H

#include "component/component.h"
#include "control/input.h"

using namespace std;

class Transform;
class Camera;
class Material;
class LoginScene:public Component{
public:
    LoginScene();

    void Awake();

    void Update();

private:
    /// 创建鱼缸
    void CreateFishSoupPot();

    /// 创建用于渲染文字图像的2个三角形
    void CreateQuad();
private:
    Transform* transform_fishsoup_pot_;
    Material* material;

    Transform* transform_camera_1_;
    Camera*     camera_1_;
    Transform* transform_camera_2_;
    Camera*     camera_2_;
    vec2_ushort last_frame_mouse_position_;//上一帧的鼠标位置
};


#endif //UNTITLED_LOGINSCENE_H
