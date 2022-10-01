//
// Created by captain on 2021/11/21.
//

#ifndef UNTITLED_ANIMATION_H
#define UNTITLED_ANIMATION_H

#include <string>
#include <unordered_map>
#include "component/component.h"


class AnimationClip;
/// 骨骼动画
class Animation:public Component {
public:
    Animation();
    ~Animation();

    /// 加载 skeleton_anim 文件
    /// \param path skeleton_anim 文件路径
    /// \param alias_name 别名，给这个动画起一个别名，方便查找
    void LoadAnimationClipFromFile(const char *path,const char* alias_name);

    /// 播放动画
    /// \param alias_name 动画别名
    void Play(const char* alias_name);

    /// 获取当前播放的动画
    AnimationClip* current_animation_clip() const{return current_animation_clip_;}

public:
    // 刷帧
    void Update() override;

private:
    /// 动画列表
    std::unordered_map<std::string,AnimationClip*> animation_clips_map_;
    /// 当前播放的动画
    AnimationClip* current_animation_clip_;

RTTR_ENABLE();
};


#endif //UNTITLED_ANIMATION_H
