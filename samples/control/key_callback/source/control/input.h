//
// Created by captain on 2021/6/20.
//

#ifndef UNTITLED_INPUT_H
#define UNTITLED_INPUT_H

#include <unordered_map>

class Input {
public:
    /// 判断当前帧 键盘按键是否按下
    /// \param key_code
    /// \return
    static bool GetKey(unsigned short key_code);

    /// 判断当前帧 键盘按键是否处于按下状态
    /// \param key_code
    /// \return
    static bool GetKeyDown(unsigned short key_code);

    /// 判断当前帧 键盘按键是否按下并松开
    /// \param key_code
    /// \return
    static bool GetKeyUp(unsigned short key_code);

    /// 记录按键事件，键盘按下记录数+1，键盘弹起记录数-1，当记录数为0，说明此时没有按键。
    /// \param key_code
    /// \param action 0松手 1按下 2持续按下
    static void RecordKey(int key_code, unsigned char key_action);

    /// 刷帧
    static void Update();
private:
    static std::unordered_map<unsigned short,unsigned char> key_event_map_;//按键状态 0弹起 1按下 2持续按下
};


#endif //UNTITLED_INPUT_H
