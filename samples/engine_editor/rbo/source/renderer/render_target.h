//
// Created by captainchen on 2022/8/30.
//

#ifndef RBO_RENDER_TARGET_H
#define RBO_RENDER_TARGET_H


class RenderTarget {
public:
    enum RenderTargetType{
        None,
        RenderTexture,
        RenderBuffer
    };

    RenderTarget(RenderTargetType render_target_type);
    virtual ~RenderTarget();

    /// 初始化RenderTexture，在GPU生成帧缓冲区对象(FrameBufferObject)
    /// \param width
    /// \param height
    virtual void Init(unsigned short width,unsigned short height);

    /// 使用
    virtual void Bind();

    virtual void UnBind();

    /// 渲染目标类型
    /// \return
    RenderTargetType render_target_type(){
        return render_target_type_;
    }

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

protected:
    RenderTargetType render_target_type_;//当前渲染目标类型
    unsigned short width_;
    unsigned short height_;
    unsigned int frame_buffer_object_handle_;//关联的FBO Handle
    bool in_use_;//正在被使用
};


#endif //RBO_RENDER_TARGET_H
