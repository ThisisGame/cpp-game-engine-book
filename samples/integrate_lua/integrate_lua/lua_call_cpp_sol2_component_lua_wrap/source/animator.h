//
// Created by captain on 2021/9/23.
//

#ifndef TEST_ANIMATOR_H
#define TEST_ANIMATOR_H

#include <iostream>
#include <rttr/registration>
#include "component.h"


class Animator:public Component
{
public:
    Animator(): Component(){}
    ~Animator(){}
public:
    void Awake() override{
        std::cout<<"Cpp.Animator Awake"<<std::endl;
        Component::Awake();
    }

    void Update() override{
        std::cout<<"Cpp.Animator Update"<<std::endl;
        Component::Update();
    }

RTTR_ENABLE();
};


#endif //TEST_ANIMATOR_H
