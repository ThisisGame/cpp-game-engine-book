//
// Created by captain on 6/4/2022.
//

#ifndef INTEGRATE_PHYSX_PHYSIC_ERROR_CALL_BACK_H
#define INTEGRATE_PHYSX_PHYSIC_ERROR_CALL_BACK_H

#include "PxPhysicsAPI.h"
#include "foundation/include/PsString.h"
#include "foundation/include/PsThread.h"
#include "core/utils/debug.h"

using namespace physx;

/**
\brief default implementation of the error callback

This class is provided in order to enable the SDK to be started with the minimum of user code. Typically an application
will use its own error callback, and log the error to file or otherwise make it visible. Warnings and error messages from
the SDK are usually indicative that changes are required in order for PhysX to function correctly, and should not be ignored.
*/

class PhysicErrorCallback : public PxErrorCallback
{
public:
    PhysicErrorCallback(){}
    ~PhysicErrorCallback(){}

    void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override{
        const char* errorCode = NULL;

        switch (code)
        {
            case PxErrorCode::eNO_ERROR:
                errorCode = "no error";
                break;
            case PxErrorCode::eINVALID_PARAMETER:
                errorCode = "invalid parameter";
                break;
            case PxErrorCode::eINVALID_OPERATION:
                errorCode = "invalid operation";
                break;
            case PxErrorCode::eOUT_OF_MEMORY:
                errorCode = "out of memory";
                break;
            case PxErrorCode::eDEBUG_INFO:
                errorCode = "info";
                break;
            case PxErrorCode::eDEBUG_WARNING:
                errorCode = "warning";
                break;
            case PxErrorCode::ePERF_WARNING:
                errorCode = "performance warning";
                break;
            case PxErrorCode::eABORT:
                errorCode = "abort";
                break;
            case PxErrorCode::eINTERNAL_ERROR:
                errorCode = "internal error";
                break;
            case PxErrorCode::eMASK_ALL:
                errorCode = "unknown error";
                break;
        }

        PX_ASSERT(errorCode);
        if(errorCode)
        {
            char buffer[1024];
            sprintf(buffer, "%s (%d) : %s : %s\n", file, line, errorCode, message);

            DEBUG_LOG_ERROR(buffer);

            // in debug builds halt execution for abort codes
            PX_ASSERT(e != PxErrorCode::eABORT);

            // in release builds we also want to halt execution
            // and make sure that the error message is flushed
            while (code == PxErrorCode::eABORT)
            {
                DEBUG_LOG_ERROR(buffer);
                physx::shdfnd::Thread::sleep(1000);
            }
        }
    }
};
#endif //INTEGRATE_PHYSX_PHYSIC_ERROR_CALL_BACK_H

