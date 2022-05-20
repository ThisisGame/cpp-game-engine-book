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
PxPvd*                  gPvd        = NULL;

PxTransform gBoxOnTheGroundTransform(PxVec3(0, 1.0f, 0));
PxBoxGeometry gBoxOnTheGroundGeometry(0.5f,0.5f,0.5f);

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
void CreateGround(){
    //~en Create Physx Material.
    //~zh 创建物理材质
    PxMaterial* planeMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.01f);

    //~en Create Plane,add to scene.
    //~zh 创建地板
    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *planeMaterial);
    groundPlane->setName("Ground");
    gScene->addActor(*groundPlane);
}

//~en Create ball
//~zh 创建球
void CreateBall(){
    //~en Create RigidBody,pos is (0,10,0)
    //~zh 创建刚体，坐标是 (0,10,0)
    PxRigidDynamic* body = gPhysics->createRigidDynamic(PxTransform(PxVec3(0, 10, 0)));
    body->setName("Ball");

    //~en Create Physx Material.
    //~zh 创建小球的物理材质
    PxMaterial* ballMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.98f);

    //~en Set ball material restitution combine mode. When ball hit the floor, choose the larger, smaller, or average of the two.
    //~zh 设置小球材质的弹性系数计算模式，小球与地板碰撞时，弹性系数是取两者大的、小的、还是平均。
    ballMaterial->setRestitutionCombineMode(PxCombineMode::eMAX);

    //~en Set rigid body sharp
    //~zh 设置刚体形状，一个球。
    float radius = 0.5f;
    PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), *ballMaterial);
    body->attachShape(*shape);
    shape->release();

    //~en calculate mass,mass = volume * density
    //~zh 根据体积、密度计算质量
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);

    gScene->addActor(*body);
}

//~zh 场景查询
//~en scene query
void SceneQuery(int frame){
    //~zh 射线检测：从指定位置发出射线，检测小球掉落。
    //~en Returns the first rigid actor that is hit along the ray.
    PxVec3 origin(0,0.5f,10);
    PxVec3 uintDir(0,0,-1);
    PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;
    PxRaycastHit raycastHit;
    if(PxSceneQueryExt::raycastSingle(*gScene, origin, uintDir, 20, hitFlags, raycastHit)){
        printf("frame %d,raycastHit.position:(%f,%f,%f)\n", frame, raycastHit.position.x, raycastHit.position.y, raycastHit.position.z);
    }

    //~zh 重叠检测：检测是否有对象与指定几何对象重叠。
    //~en Test returning, for a given geometry, any overlapping object in the scene.
    PxOverlapHit overlapHit;
    if(PxSceneQueryExt::overlapAny(*gScene,PxBoxGeometry(0.5f,0.5f,0.5f),PxTransform(PxVec3(0,5.0f,0)),overlapHit)){
        printf("frame %d,overlapHit:%s\n",frame,overlapHit.actor->getName());
    }

    //~zh 横扫检测：向前发射一个小球检测碰撞。
    //~en sweep:fire a small ball forward to detect collision.
    PxSweepHit sweepHit;
    if(PxSceneQueryExt::sweepSingle(*gScene,PxSphereGeometry(0.5f), PxTransform(PxVec3(0,9.0f,10)),uintDir,20,hitFlags,sweepHit)){
        printf("frame %d,sweepHit:%s\n",frame,sweepHit.actor->getName());
    }
}

//~en simulate game engine update
//~zh 模拟游戏引擎update
void Simulate(){
    static const PxU32 frameCount = 100;
    for(PxU32 i=1; i<=frameCount; i++) {
        gScene->simulate(1.0f/60.0f);
        gScene->fetchResults(true);
        SceneQuery(i);
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
    CreateGround();

    //~en Create ball
    //~zh 创建球
    CreateBall();

    //~en simulate game engine update
    //~zh 模拟游戏引擎update
    Simulate();

    CleanupPhysics();

    return 0;
}