//
// Created by cp on 2022/4/23.
//

#include "physics.h"

PxDefaultAllocator		Physics::px_allocator_;
PxDefaultErrorCallback	Physics::px_error_callback_;
PxFoundation*			Physics::px_foundation_;
PxPhysics*				Physics::px_physics_;
PxDefaultCpuDispatcher*	Physics::px_cpu_dispatcher_;
PxScene*		        Physics::px_scene_;
PxPvd*                  Physics::px_pvd_;

void Physics::Init() {
    //~en Creates an instance of the foundation class,The foundation class is needed to initialize higher level SDKs.only one instance per process.
    //~zh 创建Foundation实例。
    px_foundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, px_allocator_, px_error_callback_);

    //~en Connect to pvd(PhysX Visual Debugger).
    //~zh 连接PVD
    px_pvd_ = PxCreatePvd(*px_foundation_);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    px_pvd_->connect(*transport,PxPvdInstrumentationFlag::eALL);

    //~en Creates an instance of the physics SDK.
    //~zh 创建Physx SDK实例
    px_physics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *px_foundation_, PxTolerancesScale(),true,px_pvd_);
}

void Physics::FixedUpdate() {
    px_scene_->simulate(1.0f / 60.0f);
    px_scene_->fetchResults(true);
}

PxScene* Physics::CreatePxScene() {
    //~zh 创建Physx Scene
    PxSceneDesc sceneDesc(px_physics_->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    px_cpu_dispatcher_ = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher	= px_cpu_dispatcher_;
    sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
    PxScene* px_scene = px_physics_->createScene(sceneDesc);
    //~zh 设置PVD
    PxPvdSceneClient* pvd_client = px_scene->getScenePvdClient();
    if(pvd_client)
    {
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    return px_scene;
}

PxRigidDynamic* Physics::CreateRigidDynamic(const glm::vec3& pos,const char* name){
    PxRigidDynamic* body = px_physics_->createRigidDynamic(PxTransform(PxVec3(0, 10, 0)));
    body->setName(name);
    px_scene_->addActor(*body);
    return body;
}

PxRigidStatic* Physics::CreateRigidStatic(const glm::vec3 &pos, const char *name) {
    PxRigidStatic* body = px_physics_->createRigidStatic(PxTransform(PxVec3(0, 10, 0)));
    body->setName(name);
    px_scene_->addActor(*body);
    return body;
}

