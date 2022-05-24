//
// Created by captainchen on 2021/9/16.
//

#ifndef TEST_COMPONENT_H
#define TEST_COMPONENT_H

#include <rttr/registration>

using namespace rttr;

class GameObject;
class Component {
public:
    Component():game_object_(nullptr){};
    virtual ~Component(){};

    GameObject* game_object(){return game_object_;}
    void set_game_object(GameObject* game_object){game_object_=game_object;}

    virtual void Awake(){};
    virtual void Update(){};
private:
    GameObject* game_object_;

RTTR_ENABLE();
};


#endif //TEST_COMPONENT_H
