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
    /// 加载wav音频文件
    void CreateSounds();
    /// 创建玩家
    void CreatePlayer();
private:
    Transform*      transform_camera_1_= nullptr;
    Camera*         camera_1_= nullptr;
    vec2_ushort     last_frame_mouse_position_={0,0};//上一帧的鼠标位置

    Transform*      transform_player_= nullptr;
};


#endif //UNTITLED_LOGINSCENE_H
