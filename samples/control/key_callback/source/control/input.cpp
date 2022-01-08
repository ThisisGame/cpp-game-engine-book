//
// Created by captain on 2021/6/20.
//

#include "input.h"
#include <iostream>
#include "key_code.h"

std::unordered_map<unsigned short,unsigned char> Input::key_event_map_;

bool Input::GetKey(unsigned char key_code) {
    return key_event_map_.count(key_code)>0;
}

bool Input::GetKeyDown(unsigned char key_code) {
    if(key_event_map_.count(key_code)==0){
        return false;
    }
    return key_event_map_[key_code]!=KEY_ACTION_UP;
}

bool Input::GetKeyUp(unsigned char key_code) {
    if(key_event_map_.count(key_code)==0){
        return false;
    }
    return key_event_map_[key_code]==KEY_ACTION_UP;
}

void Input::RecordKey(unsigned short key_code, unsigned char key_action) {
    key_event_map_[key_code]=key_action;
}

void Input::Update() {
    for(auto iterator=key_event_map_.begin(); iterator != key_event_map_.end();) {
        if(iterator->second == KEY_ACTION_UP) {
            iterator = key_event_map_.erase(iterator);    //删除元素，返回值指向已删除元素的下一个位置
        } else {
            ++iterator;    //指向下一个位置
        }
    }
}
