//
// Created by cp on 2022/4/23.
//

#include "physics.h"
#include "utils/debug.h"

PxDefaultAllocator		Physics::px_allocator_;
PhysicErrorCallback	    Physics::physic_error_callback_;
SimulationEventCallback Physics::simulation_event_callback_;
PxFoundation*			Physics::px_foundation_;
PxPhysics*				Physics::px_physics_;
PxDefaultCpuDispatcher*	Physics::px_cpu_dispatcher_;
PxScene*		        Physics::px_scene_;
PxPvd*                  Physics::px_pvd_;
bool                    Physics::enable_ccd_=true;

//~zh 设置在碰撞发生时，Physx需要做的事情
//~en Set the actions when collision occurs,Physx needs to do.
static	PxFilterFlags SimulationFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,PxFilterObjectAttributes attributes1, PxFilterData filterData1,PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {

    //~zh eNOTIFY_TOUCH_FOUND:当碰撞发生时处理回调。 eNOTIFY_TOUCH_LOST:当碰撞结束时处理回调。
    //~en eNOTIFY_TOUCH_FOUND:When collision occurs,process callback. eNOTIFY_TOUCH_LOST:When collision ends,process callback.
    pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;

    //~zh Trigger的意思就是不处理物理碰撞，只是触发一个回调函数。
    //~en Trigger means that the physical collision is not processed,only a callback function is triggered.
    bool isTrigger=filterData0.word0==1 || filterData1.word0==1;
    if(isTrigger) {
        pairFlags = pairFlags ^ PxPairFlag::eSOLVE_CONTACT;
    }

    if(Physics::enable_ccd()){
        //~zh 场景启用CCD后，还需要指定碰撞时使用CCD，并且处理回调。
        //~en When the scene is enabled CCD, you need to specify the collision to use CCD and handle the callback.
        pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT|PxPairFlag::eNOTIFY_TOUCH_CCD;
    }
    return PxFilterFlags();
}


void Physics::Init() {
    //~en Creates an instance of the foundation class,The foundation class is needed to initialize higher level SDKs.only one instance per process.
    //~zh 创建Foundation实例。
    px_foundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, px_allocator_, physic_error_callback_);

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
    //~en set physx event callback,such as trigger,collision,etc.
    //~zh 设置事件回调，用于接收物理事件，如Awake/Trigger等
    sceneDesc.simulationEventCallback = &simulation_event_callback_;
    //~zh 设置在碰撞发生时，Physx需要做的事情
    //~en Set the actions when collision occurs,Physx needs to do.
    sceneDesc.filterShader	= SimulationFilterShader;
    if(enable_ccd_){
        //~zh 启用CCD
        //~en Enable CCD
        sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    }
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

bool Physics::RaycastSingle(glm::vec3& origin,glm::vec3& dir,float distance,RaycastHit* raycast_hit){
    PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;
    PxRaycastHit px_raycast_hit;
    if(PxSceneQueryExt::raycastSingle(*px_scene_, PxVec3(origin.x,origin.y,origin.z), PxVec3(dir.x,dir.y,dir.z), distance, hitFlags, px_raycast_hit)){
        raycast_hit->set_position(px_raycast_hit.position.x, px_raycast_hit.position.y, px_raycast_hit.position.z);

        GameObject* game_object= static_cast<GameObject *>(px_raycast_hit.shape->userData);
        raycast_hit->set_game_object(game_object);
        return true;
    }
    return false;
}

