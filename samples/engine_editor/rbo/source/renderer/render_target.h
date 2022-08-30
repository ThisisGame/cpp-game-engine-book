//
// Created by captainchen on 2022/8/30.
//

#ifndef RBO_RENDER_TARGET_H
#define RBO_RENDER_TARGET_H


class RenderTarget {
public:
    enum RenderTargetType{
        RenderTexture,
        RenderBuffer
    };

    RenderTarget();
    ~RenderTarget();

    /// 初始化RenderTexture，在GPU生成帧缓冲区对象(FrameBufferObject)
    /// \param width
    /// \param height
    void Init(unsigned short width,unsigned short height);

    unsigned short width(){
        return width_;
    }
    void set_width(unsigned short width){
        width_=width;
    }

    unsigned short height(){
        return height_;
    }
    void set_height(unsigned short height){
        height_=height;
    }

    unsigned int frame_buffer_object_handle(){
        return frame_buffer_object_handle_;
    }
    /// 是否正在被使用
    bool in_use(){
        return in_use_;
    }
    void set_in_use(bool in_use){
        in_use_=in_use;
    }

private:
    unsigned short width_;
    unsigned short height_;
    unsigned int frame_buffer_object_handle_;//关联的FBO Handle
    bool in_use_;//正在被使用
};


#endif //RBO_RENDER_TARGET_H
