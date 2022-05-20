//
// Created by captain on 2021/11/21.
//

#ifndef UNTITLED_ANIMATION_H
#define UNTITLED_ANIMATION_H

#include "component/component.h"

/// 骨骼动画
class Animation {
public:
    Animation();
    ~Animation();

    /// 加载 skeleton_anim 文件
    /// \param path
    void LoadFromFile(const char *path);
};


#endif //UNTITLED_ANIMATION_H
