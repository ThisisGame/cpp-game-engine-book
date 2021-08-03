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
class AudioSource;
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
    /// 加载wav音频文件
    void CreateSounds();
    /// 播放/暂停
    /// \param audio_source
    void PlayPauseSound(AudioSource* audio_source);
private:
    Transform* transform_fishsoup_pot_= nullptr;
    Material* material= nullptr;

    Transform* transform_camera_1_= nullptr;
    Camera*     camera_1_= nullptr;

    vec2_ushort last_frame_mouse_position_={0,0};//上一帧的鼠标位置

    AudioSource     *audio_source_bgm_= nullptr, *audio_source_knife_= nullptr, *audio_source_magic_= nullptr;
};


#endif //UNTITLED_LOGINSCENE_H
