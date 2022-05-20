//
// Created by captainchen on 2022/2/7.
//

#include "render_task_queue.h"

rigtorp::SPSCQueue<RenderTaskBase*> RenderTaskQueue::render_task_queue_(1024);//渲染任务队列