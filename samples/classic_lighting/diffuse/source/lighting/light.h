//
// Created by captainchen on 2022/6/6.
//

#ifndef INTEGRATE_PHYSX_LIGHT_H
#define INTEGRATE_PHYSX_LIGHT_H

#include <glm/glm.hpp>
#include "component/component.h"

//~zh 灯光
class Light: public Component {
public:
    Light();
    ~Light();

    glm::vec3 color() const{return color_;};
    void set_color(glm::vec3 color){color_ = color;};

    float intensity() const{return intensity_;};
    void set_intensity(float intensity){intensity_ = intensity;};

private:
    glm::vec3 color_;//颜色
    float intensity_;//强度

RTTR_ENABLE();
};


#endif //INTEGRATE_PHYSX_LIGHT_H
