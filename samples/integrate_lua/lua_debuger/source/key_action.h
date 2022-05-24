//
// Created by captain on 2021/9/23.
//

#ifndef TEST_KEY_ACTION_H
#define TEST_KEY_ACTION_H

typedef enum KeyAction{
    UP=0,
    DOWN=1,
    REPEAT=2
}KeyAction;

KeyAction GetKeyActionUp(){
    return KeyAction::UP;
}

KeyAction GetKeyActionDown(){
    return KeyAction::DOWN;
}

#endif //TEST_KEY_ACTION_H
