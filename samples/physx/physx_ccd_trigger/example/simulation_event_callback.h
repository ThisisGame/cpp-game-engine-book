//
// Created by captain on 4/7/2022.
//

#ifndef UNTITLED_SIMULATIONEVENTCALLBACK_H
#define UNTITLED_SIMULATIONEVENTCALLBACK_H

#include "PxPhysicsAPI.h"

using namespace physx;

//~en SimulationEventCallback is a PxSimulationEventCallback that is used to receive events from the PhysX SDK.
//~zh SimulationEventCallback 是一个用于从 PhysX SDK 接收事件的 PxSimulationEventCallback。
class SimulationEventCallback: public PxSimulationEventCallback {
public:
    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {
        printf("onConstraintBreak\n");
    }

    void onWake(PxActor** actors, PxU32 count) override {
        printf("onWake\n");
    }

    void onSleep(PxActor** actors, PxU32 count) override {
        printf("onSleep\n");
    }

    void onTrigger(PxTriggerPair* pairs, PxU32 count) override {
		printf("onTrigger: %d trigger pairs\n", count);
        while(count--)
        {
            const PxTriggerPair& current = *pairs++;
            if(current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                printf("Shape is entering trigger volume\n");
            }
            if(current.status & PxPairFlag::eNOTIFY_TOUCH_LOST){
                printf("Shape is leaving trigger volume\n");
            }
        }
    }

    void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) override {
        printf("onAdvance\n");
    }

    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 count) override {
		printf("onContact: %d pairs\n", count);
        while(count--) {
            const PxContactPair& current = *pairs++;

            //~zh 判断Shape附加数据为1表示Trigger。
            //~en If the shape's user data is 1, it is a trigger.
            if(current.shapes[0]->getSimulationFilterData().word0 & 0x1) {
                printf("Shape 0 is a trigger\n");
            }
            if(current.shapes[1]->getSimulationFilterData().word0 & 0x1) {
                printf("Shape 1 is a trigger\n");
            }

            if(current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND|PxPairFlag::eNOTIFY_TOUCH_CCD)) {
                printf("onContact Shape is entering volume\n");
            }
            if(current.events & (PxPairFlag::eNOTIFY_TOUCH_LOST)) {
                printf("onContact Shape is leaving volume\n");
            }
        }
    }
};


#endif //UNTITLED_SIMULATIONEVENTCALLBACK_H
