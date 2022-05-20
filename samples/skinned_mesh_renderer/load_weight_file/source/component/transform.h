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

    glm::vec3 position() const {return position_;}
    glm::vec3 rotation() const {return rotation_;}
    glm::vec3 scale() const {return scale_;}

    void set_position(glm::vec3 position){position_=position;}
    void set_rotation(glm::vec3 rotation){rotation_=rotation;}
    void set_scale(glm::vec3 scale){scale_=scale;}

private:
    glm::vec3 position_;
    glm::vec3 rotation_;
    glm::vec3 scale_;
};
#endif //UNTITLED_TRANSFORM_H
