//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDER_TASK_CONSUMER_H
#define UNTITLED_RENDER_TASK_CONSUMER_H

class RenderTaskConsumerBase;

/// 渲染任务消费端
class RenderTaskConsumer {
public:
    static void Init(RenderTaskConsumerBase* instance);

    static RenderTaskConsumerBase* Instance();

    static void Exit();
private:
    static RenderTaskConsumerBase* instance_;
};



#endif //UNTITLED_RENDER_TASK_CONSUMER_H
