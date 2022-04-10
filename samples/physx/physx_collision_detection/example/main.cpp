#include <ctype.h>

#include "simulation_event_callback.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
SimulationEventCallback gSimulationEventCallback;
PxScene*				gScene		= NULL;
PxPvd*                  gPvd        = NULL;

//~en Init Physx
//~zh 初始化Physx
void InitPhysics()
{
    //~en Creates an instance of the foundation class,The foundation class is needed to initialize higher level SDKs.only one instance per process.
    //~zh 创建Foundation实例。
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    //~en Connect to pvd(PhysX Visual Debugger).
    //~zh 连接PVD
    gPvd = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

    //~en Creates an instance of the physics SDK.
    //~zh 创建Physx SDK实例
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);
}


//~zh 设置在碰撞发生时，Physx需要做的事情
//~en Set the actions when collision occurs,Physx needs to do.
static	PxFilterFlags SimulationFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,PxFilterObjectAttributes attributes1, PxFilterData filterData1,PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
    pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;
    return PxFilterFlags();
}

//~en Create Scene
//~zh 创建Scene
void CreateScene(){
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -0.98f, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher	= gDispatcher;
    //~zh 指定事件回调
    //~en Specify the event callback
    sceneDesc.simulationEventCallback = &gSimulationEventCallback;
    //~zh 设置在碰撞发生时，Physx需要做的事情
    //~en Set the actions when collision occurs,Physx needs to do.
    sceneDesc.filterShader	= SimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if(pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

//~en Create wall,add to scene.
//~zh 创建墙，并添加到场景中
void CreateWall(){
    //~en Create RigidBody,pos is (0,10,0)
    //~zh 创建刚体，坐标是 (0,10,0)
    PxRigidStatic* body = gPhysics->createRigidStatic(PxTransform(PxVec3(0, 10, 0)));

    //~en Create Physx Material.
    //~zh 创建物理材质
    PxMaterial* wallMaterial = gPhysics->createMaterial(1.0f, 1.0f, 0.0f);

    //~en Create wall shape.
    //~zh 创建墙体形状
    const PxVec3 halfExtent(0.1f, 10.0f, 10.0f);
//    PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent), *wallMaterial);
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent), *wallMaterial,false,PxShapeFlag::eVISUALIZATION | PxShapeFlag::eTRIGGER_SHAPE);

    //~en Add shape to body.
    //~zh 设置刚体形状，长方体的一面墙。
    body->attachShape(*shape);
    shape->release();

    //~en Add body to scene.
    //~zh 将刚体添加到场景中
    gScene->addActor(*body);
}

//~zh 创建小球，并添加到场景中
//~en Create ball,add to scene.
void CreateBall(){
    //~en Create RigidBody,pos is (10,0,0)
    //~zh 创建刚体，坐标是 (10,0,0)
    PxRigidDynamic* body = gPhysics->createRigidDynamic(PxTransform(PxVec3(10, 5, 0)));
    body->setLinearVelocity(PxVec3(-14.0f, 0.0f, 0.0f));

    //~en Create Physx Material.
    //~zh 创建小球的物理材质
    PxMaterial* ballMaterial = gPhysics->createMaterial(0.5f, 0.5f, 1.0f);

    //~en Set rigid body sharp
    //~zh 设置刚体形状，一个球。
    float radius = 0.5f;
    PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), *ballMaterial);
    body->attachShape(*shape);
    shape->release();

    //~en calculate mass,mass = volume * density
    //~zh 根据体积、密度计算质量
    PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);

    gScene->addActor(*body);
}

//~en simulate game engine update
//~zh 模拟游戏引擎update
void Simulate(){
    static const PxU32 frameCount = 1000;
    for(PxU32 i=0; i<frameCount; i++) {
        gScene->simulate(1.0f/60.0f);
        gScene->fetchResults(true);
    }
}

void CleanupPhysics()
{
    PX_RELEASE(gScene);
    PX_RELEASE(gDispatcher);
    PX_RELEASE(gPhysics);
    if(gPvd)
    {
        PxPvdTransport* transport = gPvd->getTransport();
        gPvd->release();	gPvd = NULL;
        PX_RELEASE(transport);
    }
    PX_RELEASE(gFoundation);

    printf("test physx trigger done.\n");
}

int main()
{
    //~en Init Physx
    //~zh 初始化Physx，创建场景，球体。
    InitPhysics();

    //~en Create Scene
    //~zh 创建Scene
    CreateScene();

    //~en Create Plane,add to scene.
    //~zh 创建地板
    CreateWall();

    //~en Create ball
    //~zh 创建球
    CreateBall();

    //~en simulate game engine update
    //~zh 模拟游戏引擎update
    Simulate();

    CleanupPhysics();

    return 0;
}