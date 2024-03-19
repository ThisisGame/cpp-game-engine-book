//
// Created by captain on 2021/6/9.
//

#ifndef UNTITLED_TRANSFORM_H
#define UNTITLED_TRANSFORM_H

#include "component.h"
#include <glm/glm.hpp>

class Transform : public Component{
public:
    Transform();
    ~Transform();

    glm::vec3 local_position() const {return local_position_;}
    glm::vec3 local_rotation() const {return local_rotation_;}
    glm::vec3 local_scale() const {return local_scale_;}

    void set_local_position(glm::vec3 local_position){ local_position_=local_position;}
    void set_local_rotation(glm::vec3 local_rotation){ local_rotation_=local_rotation;}
    void set_local_scale(glm::vec3 local_scale){ local_scale_=local_scale;}

    /// 获取世界坐标，即叠加了所有父节点的坐标
    /// \return
    glm::vec3 position();
    /// 获取世界旋转，即叠加了所有父节点的旋转
    /// \return
    glm::vec3 rotation();
    /// 获取世界缩放，即叠加了所有父节点的缩放
    /// \return
    glm::vec3 scale();

private:
    glm::vec3 local_position_;
    glm::vec3 local_rotation_;
    glm::vec3 local_scale_;

RTTR_ENABLE();
};
#endif //UNTITLED_TRANSFORM_H
