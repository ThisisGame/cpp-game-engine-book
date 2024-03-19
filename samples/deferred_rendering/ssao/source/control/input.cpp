//
// Created by captain on 2021/6/20.
//

#include "input.h"
#include <iostream>
#include "key_code.h"

std::unordered_map<unsigned short,unsigned short> Input::key_event_map_;
glm::vec2 Input::mouse_position_={0,0};
short Input::mouse_scroll_=0;

bool Input::GetKey(unsigned short key_code) {
    return key_event_map_.count(key_code)>0;
}

bool Input::GetKeyDown(unsigned short key_code) {
    if(key_event_map_.count(key_code)==0){
        return false;
    }
    return key_event_map_[key_code]!=KEY_ACTION_UP;
}

bool Input::GetKeyUp(unsigned short key_code) {
    if(key_event_map_.count(key_code)==0){
        return false;
    }
    return key_event_map_[key_code]==KEY_ACTION_UP;
}

bool Input::GetMouseButton(unsigned short mouse_button_index) {
    return GetKey(mouse_button_index);
}

bool Input::GetMouseButtonDown(unsigned short mouse_button_index) {
    return GetKeyDown(mouse_button_index);
}

bool Input::GetMouseButtonUp(unsigned short mouse_button_index) {
    return GetKeyUp(mouse_button_index);
}

void Input::RecordKey(unsigned short key_code, unsigned short key_action) {
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

    mouse_scroll_ = 0;
}