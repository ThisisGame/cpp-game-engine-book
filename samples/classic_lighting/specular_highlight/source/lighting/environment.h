//
// Created by captainchen on 2022/6/7.
//

#ifndef INTEGRATE_PHYSX_ENVIRONMENT_H
#define INTEGRATE_PHYSX_ENVIRONMENT_H

#include <glm/glm.hpp>

/// 环境配置
class Environment {
public:
    Environment();

    /// 设置环境颜色
    void set_ambient_color(const glm::vec3 &ambient_color){ambient_color_= ambient_color;}

    /// 获取环境颜色
    const glm::vec3 &ambient_color() const{return ambient_color_;}

    /// 设置环境强度
    void set_ambient_color_intensity(float ambient_color_intensity){ambient_color_intensity_ = ambient_color_intensity;}

    /// 获取环境强度
    float ambient_color_intensity() const{return ambient_color_intensity_;}

private:
    glm::vec3 ambient_color_;// 环境颜色
    float ambient_color_intensity_;// 环境颜色强度
};


#endif //INTEGRATE_PHYSX_ENVIRONMENT_H
