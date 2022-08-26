//
// Created by captain on 8/24/2022.
//

#ifndef DRAW_RTT_RECT_TRANSFORM_H
#define DRAW_RTT_RECT_TRANSFORM_H

#include "component/transform.h"

class RectTransform : public Transform {
public:
    RectTransform();
    ~RectTransform();

    /// 获取宽高
    /// \return
    glm::vec2 rect(){
        return rect_;
    }

    void set_rect(glm::vec2 rect){
        rect_=rect;
    }
private:
    glm::vec2 rect_;//宽高

RTTR_ENABLE(Transform);
};


#endif //DRAW_RTT_RECT_TRANSFORM_H
