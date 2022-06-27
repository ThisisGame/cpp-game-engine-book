//
// Created by captain on 4/28/2022.
//

#ifndef ENGINE_LIGHTING_DIRECTIONAL_LIGHT_H
#define ENGINE_LIGHTING_DIRECTIONAL_LIGHT_H

#include <rttr/registration>
#include "light.h"

using namespace rttr;

class DirectionalLight : public Light {
public:
    DirectionalLight();
    ~DirectionalLight();

public:
    void Update() override;

RTTR_ENABLE(Light);
};


#endif //ENGINE_LIGHTING_DIRECTIONAL_LIGHT_H
