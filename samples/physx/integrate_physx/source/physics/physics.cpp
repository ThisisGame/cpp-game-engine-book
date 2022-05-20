//
// Created by cp on 2022/4/23.
//

#include "physics.h"
#include "utils/debug.h"

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

    px_scene_ = CreatePxScene();
}

void Physics::FixedUpdate() {
    if(px_scene_==nullptr) {
        DEBUG_LOG_ERROR("px_scene_==nullptr,please call Physics::CreatePxScene() first");
        return;
    }
    px_scene_->simulate(1.0f / 60.0f);
    px_scene_->fetchResults(true);
}

PxScene* Physics::CreatePxScene() {
    if(px_physics_==nullptr) {
        DEBUG_LOG_ERROR("px_physics_==nullptr,please call Physics::Init() first");
        return nullptr;
    }
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
    PxRigidDynamic* body = px_physics_->createRigidDynamic(PxTransform(PxVec3(pos.x,pos.y,pos.z)));
    body->setName(name);
    px_scene_->addActor(*body);
    return body;
}

PxRigidStatic* Physics::CreateRigidStatic(const glm::vec3 &pos, const char *name) {
    PxRigidStatic* body = px_physics_->createRigidStatic(PxTransform(PxVec3(pos.x,pos.y,pos.z)));
    body->setName(name);
    px_scene_->addActor(*body);
    return body;
}

PxMaterial* Physics::CreateMaterial(float static_friction, float dynamic_friction, float restitution){
    PxMaterial* material = px_physics_->createMaterial(static_friction, dynamic_friction, restitution);
    return material;
}

PxShape* Physics::CreateSphereShape(float radius, PxMaterial* material){
    PxShape* shape = px_physics_->createShape(PxSphereGeometry(radius), *material);
    return shape;
}

PxShape* Physics::CreateBoxShape(const glm::vec3& size, PxMaterial* material){
    PxShape* shape = px_physics_->createShape(PxBoxGeometry(size.x/2,size.y/2,size.z/2), *material);
    return shape;
}

