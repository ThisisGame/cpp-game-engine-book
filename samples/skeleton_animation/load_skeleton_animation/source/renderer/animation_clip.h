//
// Created by captain on 2021/11/21.
//

#ifndef UNTITLED_ANIMATION_CLIP_H
#define UNTITLED_ANIMATION_CLIP_H

/// 骨骼动画片段
class AnimationClip {
public:
    AnimationClip();
    ~AnimationClip();

    /// 设置持续时间
    /// \param duration
    void set_duration(float duration){};
        this->duration_ = duration;
    }

    /// 获取持续时间
    float duration(){
        return this->duration_;
    }

private:
    /// 持续时间
    float duration_;
};


#endif //UNTITLED_ANIMATION_CLIP_H
