//
// Created by captainchen on 2021/12/6.
//

#ifndef TEST_UI_CAMERA_H
#define TEST_UI_CAMERA_H

#include "camera.h"

#include <rttr/registration>
using namespace rttr;

class UICamera:public Camera {
public:
    UICamera();
    ~UICamera();

RTTR_ENABLE();
};


#endif //TEST_UI_CAMERA_H
