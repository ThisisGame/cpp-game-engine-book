//
// Created by cp on 2022/4/23.
//

#ifndef UNTITLED_RIGID_BODY_H
#define UNTITLED_RIGID_BODY_H

#include "component/component.h"

// 刚体
class RigidBody : public Component{
public:
    RigidBody();
    ~RigidBody();
public:
    float density_;//密度

};


#endif //UNTITLED_RIGID_BODY_H
