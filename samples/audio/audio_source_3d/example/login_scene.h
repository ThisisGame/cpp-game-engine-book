//
// Created by captain on 2021/7/10.
//

#ifndef UNTITLED_LOGINSCENE_H
#define UNTITLED_LOGINSCENE_H

#include <string>
#include "component/component.h"
#include "control/input.h"


using namespace std;

class Transform;
class Camera;
class Material;
class AudioSource;
class LoginScene:public Component{
public:
    LoginScene();

    void Awake();

    void Update();

private:
    /// 创建音源
    void CreateAudioSource();
    /// 创建听者
    void CreateAudioListener();
private:
    Transform*      transform_camera_1_= nullptr;
    Camera*         camera_1_= nullptr;
    glm::vec2     last_frame_mouse_position_={0,0};//上一帧的鼠标位置

    Transform*      transform_player_= nullptr;
};


#endif //UNTITLED_LOGINSCENE_H
