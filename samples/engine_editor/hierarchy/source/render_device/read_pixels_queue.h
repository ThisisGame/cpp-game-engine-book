//
// Created by captain on 2022/3/4.
//

#ifndef UNTITLED_READ_PIXELS_QUEUE_H
#define UNTITLED_READ_PIXELS_QUEUE_H

#include <spscqueue/include/rigtorp/SPSCQueue.h>

/// 读取前缓冲区 储存结构
class ReadPixelsBuffer{
public:
    ReadPixelsBuffer(char* buffer){
        buffer_=buffer;
    }

    ~ReadPixelsBuffer(){
        delete buffer_;
        buffer_=nullptr;
    }
public:
    char* buffer_=nullptr;
};

/// 存储游戏截帧
class ReadPixelsQueue {
public:
    /// 添加到队列
    /// \param read_pixels_buffer
    static void Push(ReadPixelsBuffer* read_pixels_buffer){
        read_pixels_queue_.push(read_pixels_buffer);
    }

    /// 队列中是否没有了
    /// \return
    static bool Empty(){
        return read_pixels_queue_.empty();
    }

    /// 获取队列中第一个
    /// \return
    static ReadPixelsBuffer* Front(){
        return *(read_pixels_queue_.front());
    }

    /// 弹出队列中第一个
    static void Pop(){
        read_pixels_queue_.pop();
    }

    /// 获取队列中的任务数量
    static size_t Size(){
        return read_pixels_queue_.size();
    }
private:
    static rigtorp::SPSCQueue<ReadPixelsBuffer*> read_pixels_queue_;
};

#endif //UNTITLED_READ_PIXELS_QUEUE_H
