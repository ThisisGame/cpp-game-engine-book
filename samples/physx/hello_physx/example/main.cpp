#include <ctype.h>

#include "PxPhysicsAPI.h"

using namespace physx;

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene		= NULL;

PxMaterial*				gMaterial	= NULL;

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

//~en Create Scene
//~zh 创建Scene
void CreateScene(){
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher	= gDispatcher;
    sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if(pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

//~en Create Plane,add to scene.
//~zh 创建地板
void CreatePlane(){
    //~en Create Physx Material.
    //~zh 创建物理材质
    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    //~en Create Plane,add to scene.
    //~zh 创建地板
    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
    gScene->addActor(*groundPlane);
}

//~en Create ball
//~zh 创建球
void CreateBall(){
    //~en Create RigidBody,pos is (0,10,0)
    //~zh 创建刚体，坐标是 (0,10,0)
    PxRigidDynamic* body = gPhysics->createRigidDynamic(PxTransform(PxVec3(0, 10, 0)));

    //~en Set rigidbody sharp
    //~zh 设置刚体形状，一个球。
    float radius = 0.5f;
    PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), *gMaterial);
    body->attachShape(*shape);
    shape->release();

    //~en calculate mass,mass = volume * density
    //~zh 根据体积、密度计算质量
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);

    gScene->addActor(*body);
}

//~en simulate game engine update
//~zh 模拟游戏引擎update
void Simulate(){
    static const PxU32 frameCount = 10000;
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

    printf("SnippetHelloWorld done.\n");
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
    CreatePlane();

    //~en Create ball
    //~zh 创建球
    CreateBall();

    //~en simulate game engine update
    //~zh 模拟游戏引擎update
    Simulate();

    CleanupPhysics();

    return 0;
}