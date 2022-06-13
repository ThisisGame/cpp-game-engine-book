//
// Created by captain on 4/7/2022.
//

#ifndef UNTITLED_SIMULATIONEVENTCALLBACK_H
#define UNTITLED_SIMULATIONEVENTCALLBACK_H

#include "PxPhysicsAPI.h"
#include "component/game_object.h"
#include "utils/debug.h"

using namespace physx;

//~en SimulationEventCallback is a PxSimulationEventCallback that is used to receive events from the PhysX SDK.
//~zh SimulationEventCallback 是一个用于从 PhysX SDK 接收事件的 PxSimulationEventCallback。
class SimulationEventCallback: public PxSimulationEventCallback {
public:
    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {
        DEBUG_LOG_INFO("onConstraintBreak");
    }

    void onWake(PxActor** actors, PxU32 count) override {
        DEBUG_LOG_INFO("onWake");
    }

    void onSleep(PxActor** actors, PxU32 count) override {
        DEBUG_LOG_INFO("onSleep");
    }

    void onTrigger(PxTriggerPair* pairs, PxU32 count) override {
		DEBUG_LOG_INFO("onTrigger: {} trigger pairs", count);
        while(count--)
        {
            const PxTriggerPair& current = *pairs++;
            if(current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                DEBUG_LOG_INFO("Shape is entering trigger volume");
            }
            if(current.status & PxPairFlag::eNOTIFY_TOUCH_LOST){
                DEBUG_LOG_INFO("Shape is leaving trigger volume");
            }
        }
    }

    void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) override {
        DEBUG_LOG_INFO("onAdvance");
    }

    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 count) override {
//		DEBUG_LOG_INFO("onContact: {} pairs", count);
        while(count--) {
            const PxContactPair& current = *pairs++;

            for (int i = 0; i < 2; ++i) {
                PxShape* shape=current.shapes[i];
                PxShape* another_shape=current.shapes[i^1];
                //~zh 判断Shape附加数据为1表示Trigger。
                //~en If the shape's user data is 1, it is a trigger.
                bool is_trigger=shape->getSimulationFilterData().word0 & 0x1;
                if (!is_trigger){
                    continue;
                }
                GameObject* gameObject= static_cast<GameObject *>(shape->userData);
//                DEBUG_LOG_INFO("Shape is a trigger,gameObject:{}", gameObject->name());
                GameObject* another_game_object= static_cast<GameObject *>(another_shape->userData);
                if(another_game_object->active()==false){
                    continue;
                }

                if(current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND|PxPairFlag::eNOTIFY_TOUCH_CCD)) {
//                    DEBUG_LOG_INFO("onContact Shape is entering volume");
                    //通知相交的另外一个物体进入
                    another_game_object->ForeachComponent([gameObject](Component* component){
                        component->OnTriggerEnter(gameObject);
                    });
                }

                if(current.events & (PxPairFlag::eNOTIFY_TOUCH_LOST)) {
//                    DEBUG_LOG_INFO("onContact Shape is leaving volume");
                    //通知相交的另外一个物体离开
                    another_game_object->ForeachComponent([gameObject](Component* component){
                        component->OnTriggerExit(gameObject);
                    });
                }
            }
        }
    }
};


#endif //UNTITLED_SIMULATIONEVENTCALLBACK_H
