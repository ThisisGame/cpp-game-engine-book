//
// Created by captain on 4/28/2022.
//

#ifndef ENGINE_LIGHTING_POINT_LIGHT_H
#define ENGINE_LIGHTING_POINT_LIGHT_H

#include <rttr/registration>
#include "light.h"

using namespace rttr;

class PointLight : public Light {
public:
    PointLight();
    ~PointLight();

    float attenuation_constant() const{return attenuation_constant_;}
    void set_attenuation_constant(float attenuation_constant);

    float attenuation_linear() const{return attenuation_linear_;}
    void set_attenuation_linear(float attenuation_linear);

    float attenuation_quadratic() const{return attenuation_quadratic_;}
    void set_attenuation_quadratic(float attenuation_quadratic);

public:
    void Update() override;

protected:
    float attenuation_constant_;//点光衰减常数项
    float attenuation_linear_;//点光衰减一次项
    float attenuation_quadratic_;//点光衰减二次项

RTTR_ENABLE(Light);
};


#endif //ENGINE_LIGHTING_POINT_LIGHT_H
