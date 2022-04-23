//
// Created by cp on 2022/4/23.
//

#ifndef UNTITLED_PHYSICS_SCENE_H
#define UNTITLED_PHYSICS_SCENE_H

#include <PxPhysicsAPI.h>

using namespace physx;

// 物理模拟的场景单元,对Physx
class PhysicsScene {
public:
    PhysicsScene(PxScene* px_scene);
    ~PhysicsScene();

    /// 添加物理对象
    void AddActor(PxActor& actor);

private:
    PxScene* scene_;//Physx的Scene
};


#endif //UNTITLED_PHYSICS_SCENE_H
