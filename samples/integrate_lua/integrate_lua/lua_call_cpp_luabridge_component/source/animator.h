//
// Created by captain on 2021/9/23.
//

#ifndef TEST_ANIMATOR_H
#define TEST_ANIMATOR_H


class Animator:public Component
{
public:
    void Awake() override{
        std::cout<<"Animator Awake"<<std::endl;
    }

    void Update() override{
//        std::cout<<"Animator Update"<<std::endl;
    }
};


#endif //TEST_ANIMATOR_H
