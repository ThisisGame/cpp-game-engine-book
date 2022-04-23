//
// Created by cp on 2022/4/23.
//

#ifndef UNTITLED_PHYSICS_H
#define UNTITLED_PHYSICS_H

#include "physics_scene.h"

// 物理模拟管理器
class Physics {
public:
    /// 初始化
    void Init();

    /// 驱动物理模拟
    void Simulate();

    /// 创建物理模拟的场景单元
    /// \return 创建的物理场景单元
    PhysicsScene CreateScene();
};


#endif //UNTITLED_PHYSICS_H
