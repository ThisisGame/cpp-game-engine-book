//
// Created by captain on 2021/9/23.
//

#ifndef TEST_CAMERA_H
#define TEST_CAMERA_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "component.h"

class Camera:public Component
{
public:
    ~Camera(){

    }
public:
    void Awake() override{
        std::cout<<"Camera Awake"<<std::endl;
    }

    void Update() override{
//        std::cout<<"Camera Update"<<std::endl;
    }

    void set_position(glm::vec3 position){
        std::cout<<"Camera set_position:"<<glm::to_string(position)<<std::endl;
        position_=position;
    }

    glm::vec3 position(){
        return position_;
    }

private:
    glm::vec3 position_;
};


#endif //TEST_CAMERA_H
