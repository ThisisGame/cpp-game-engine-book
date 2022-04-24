//
// Created by cp on 2022/4/23.
//

#ifndef UNTITLED_PHYSICS_H
#define UNTITLED_PHYSICS_H

#include <list>
#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
using namespace physx;


// 物理模拟管理器
class Physics {
public:
    /// 初始化
    static void Init();

    /// 驱动物理模拟
    static void FixedUpdate();

    /// 创建物理模拟的场景单元
    /// \return 创建的物理场景单元
    static PxScene* CreatePxScene();

    static PxRigidDynamic* CreateRigidDynamic(const glm::vec3& pos,const char* name);

    static PxRigidStatic* CreateRigidStatic(const glm::vec3& pos,const char* name);

private:
    static PxDefaultAllocator		px_allocator_;
    static PxDefaultErrorCallback	px_error_callback_;

    static PxFoundation*			px_foundation_;
    static PxPhysics*				px_physics_;

    static PxDefaultCpuDispatcher*	px_cpu_dispatcher_;
    static PxScene*		            px_scene_;
    static PxPvd*                   px_pvd_;
};


#endif //UNTITLED_PHYSICS_H
