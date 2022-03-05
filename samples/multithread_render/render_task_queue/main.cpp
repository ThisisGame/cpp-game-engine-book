#include <stdlib.h>
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "render_task_consumer.h"
#include "render_task_producer.h"
#include "VertexData.h"
#include "ShaderSource.h"

void Render();

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

GLuint program_id_=0;
GLuint vao_=0;
int main(void)
{
    //设置错误回调
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //创建窗口
    GLFWwindow* window = glfwCreateWindow(960, 640, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //初始化渲染任务消费者(独立线程)
    RenderTaskConsumer::Init(window);

    //编译Shader任务
    RenderTaskProducer::ProduceRenderTaskCompileShader(vertex_shader_text,fragment_shader_text,program_id_);

    //创建缓冲区任务
    RenderTaskProducer::ProduceRenderTaskCreateVAO(program_id_, kPositions, sizeof(glm::vec3), kColors,
                                                   sizeof(glm::vec4), vao_);

    //主线程 渲染循环逻辑
    while (!glfwWindowShouldClose(window))
    {
        Render();

        //发出特殊任务：渲染结束
        RenderTaskProducer::ProduceRenderTaskEndFrame();

        //非渲染相关的API，例如处理系统事件，就放到主线程中。
        glfwPollEvents();
    }

    RenderTaskConsumer::Exit();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void Render(){
    //绘制任务
    RenderTaskProducer::ProduceRenderTaskDrawArray(program_id_,vao_);
}