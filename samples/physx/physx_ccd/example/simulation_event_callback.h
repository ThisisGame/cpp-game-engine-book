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
            if(current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
                printf("Shape is entering trigger volume\n");
            if(current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
                printf("Shape is leaving trigger volume\n");
        }
    }

    void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) override {
        printf("onAdvance\n");
    }

    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 count) override {
		printf("onContact: %d pairs\n", count);
        while(count--)
        {
            const PxContactPair& current = *pairs++;

            // The reported pairs can be trigger pairs or not. We only enabled contact reports for
            // trigger pairs in the filter shader, so we don't need to do further checks here. In a
            // real-world scenario you would probably need a way to tell whether one of the shapes
            // is a trigger or not. You could e.g. reuse the PxFilterData like we did in the filter
            // shader, or maybe use the shape's userData to identify triggers, or maybe put triggers
            // in a hash-set and test the reported shape pointers against it. Many options here.

            if(current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND|PxPairFlag::eNOTIFY_TOUCH_CCD))
                printf("onContact Shape is entering trigger volume\n");
            if(current.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
                printf("onContact Shape is leaving trigger volume\n");

            if(current.shapes[0]->userData && current.shapes[1]->userData)
                printf("onContact Trigger-trigger overlap detected\n");
        }
    }
};


#endif //UNTITLED_SIMULATIONEVENTCALLBACK_H
