//
// Created by captain on 4/9/2022.
//

#ifndef UNTITLED_TRIGGER_FILTER_CALLBACK_H
#define UNTITLED_TRIGGER_FILTER_CALLBACK_H

#include "PxPhysicsAPI.h"

using namespace physx;

class TriggersFilterCallback : public PxSimulationFilterCallback
{
    virtual PxFilterFlags pairFound(PxU32 pairID,PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor* a0, const PxShape* s0,PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor* a1, const PxShape* s1,PxPairFlags& pairFlags) override{
        if(s0->userData || s1->userData) {// See createTriggerShape() function
            pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
#ifdef HIGH_SPEED
            pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT|PxPairFlag::eNOTIFY_TOUCH_CCD;
#endif
        } else {
            pairFlags = PxPairFlag::eCONTACT_DEFAULT;
        }
        return PxFilterFlags();
    }

    virtual void pairLost(PxU32 pairID,PxFilterObjectAttributes attributes0,PxFilterData filterData0,PxFilterObjectAttributes attributes1,PxFilterData filterData1,bool objectRemoved)
    {
		printf("pairLost\n");
    }

    virtual	bool statusChange(PxU32& pairID, PxPairFlags& pairFlags, PxFilterFlags& filterFlags)
    {
		printf("statusChange\n");
        return false;
    }
};

#endif //UNTITLED_TRIGGER_FILTER_CALLBACK_H
