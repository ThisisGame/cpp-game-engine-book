//
// Created by captainchen on 2021/9/16.
//

#ifndef TEST_COMPONENT_H
#define TEST_COMPONENT_H


class GameObject;
class Component {
public:
    Component():game_object_(nullptr){};
    virtual ~Component(){};

    const char* type_name(){return type_name_;}
    void set_type_name(const char* type_name){type_name_=type_name;}

    GameObject* game_object(){return game_object_;}
    void set_game_object(GameObject* game_object){game_object_=game_object;}

    virtual void Awake(){};
    virtual void Update(){};
private:
    const char* type_name_;
    GameObject* game_object_;
};


#endif //TEST_COMPONENT_H
