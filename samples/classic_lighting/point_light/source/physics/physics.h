//
// Created by cp on 2022/4/23.
//

#ifndef UNTITLED_PHYSICS_H
#define UNTITLED_PHYSICS_H

#include <list>
#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include "simulation_event_callback.h"
#include "physic_error_call_back.h"
#include "raycast_hit.h"

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

    static PxMaterial* CreateMaterial(float static_friction, float dynamic_friction, float restitution);

    static PxShape* CreateSphereShape(float radius, PxMaterial* material);

    static PxShape* CreateBoxShape(const glm::vec3& size, PxMaterial* material);

    static bool enable_ccd(){return enable_ccd_;}
    static void set_enable_ccd(bool enable_ccd){enable_ccd_=enable_ccd;}

    /// 射线检测：从指定位置发出射线，返回射线击中的第一个物体信息。
    /// \param origin
    /// \param dir
    /// \param distance
    /// \param raycast_hit
    /// \return
    static bool RaycastSingle(glm::vec3& origin,glm::vec3& dir,float distance,RaycastHit* raycast_hit);
private:
    static PxDefaultAllocator		px_allocator_;
    static PhysicErrorCallback	    physic_error_callback_;
    static SimulationEventCallback  simulation_event_callback_;

    static PxFoundation*			px_foundation_;
    static PxPhysics*				px_physics_;

    static PxDefaultCpuDispatcher*	px_cpu_dispatcher_;
    static PxScene*		            px_scene_;
    static PxPvd*                   px_pvd_;

    static bool                     enable_ccd_;//连续检测。
};


#endif //UNTITLED_PHYSICS_H
