//
// Created by captainchen on 2022/6/6.
//

#include "light.h"
#include "rttr/registration"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "core/component/game_object.h"
#include "core/component/transform.h"
#include "core/renderer/mesh_renderer.h"
#include "core/renderer/material.h"
#include "core/render_device/uniform_buffer_object_manager.h"

using namespace rttr;
RTTR_REGISTRATION//注册反射
{
    registration::class_<Light>("Light")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}


Light::Light():Component() {

}

Light::~Light() {}