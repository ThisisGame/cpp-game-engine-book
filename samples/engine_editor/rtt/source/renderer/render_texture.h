//
// Created by captainchen on 2022/8/19.
//

#ifndef MULTI_LIGHT_RENDER_TEXTURE_H
#define MULTI_LIGHT_RENDER_TEXTURE_H


class RenderTexture {
public:
    RenderTexture();

    unsigned int frame_buffer_object_id(){
        return frame_buffer_object_id_;
    }
    void set_frame_buffer_object_id(unsigned int frame_buffer_object_id){
        frame_buffer_object_id_=frame_buffer_object_id;
    }

private:
    unsigned short width_;
    unsigned short height_;
    unsigned int frame_buffer_object_id_;//关联的FBO
};


#endif //MULTI_LIGHT_RENDER_TEXTURE_H
