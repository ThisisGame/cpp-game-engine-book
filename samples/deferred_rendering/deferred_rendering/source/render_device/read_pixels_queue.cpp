//
// Created by captainchen on 2022/2/7.
//

#include "read_pixels_queue.h"

rigtorp::SPSCQueue<ReadPixelsBuffer*> ReadPixelsQueue::read_pixels_queue_(1024);//渲染任务队列