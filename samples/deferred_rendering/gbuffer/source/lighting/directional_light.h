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

    virtual void set_color(glm::vec3 color) override;

    virtual void set_intensity(float intensity) override;

public:
    void Update() override;

    void OnEnable() override;

    void OnDisable() override;

private:
    static unsigned int light_count_;//灯光数量

RTTR_ENABLE(Light);
};


#endif //ENGINE_LIGHTING_DIRECTIONAL_LIGHT_H
