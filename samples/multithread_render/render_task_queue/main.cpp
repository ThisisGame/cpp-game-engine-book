#include <stdlib.h>
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "renderer.h"
#include "VertexData.h"
#include "ShaderSource.h"

void Render();

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

Renderer* renderer;
RenderTaskCompileShader* render_task_compile_shader;
GLuint program_id_=0;
int main(void)
{
    //设置错误回调
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    //创建窗口
    GLFWwindow* window = glfwCreateWindow(960, 640, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //创建Renderer(独立线程)
    renderer=new Renderer(window);

    //编译Shader任务
    render_task_compile_shader=new RenderTaskCompileShader();
    {
        render_task_compile_shader->render_command_=RenderCommand::COMPILE_SHADER;
        //构造参数
        render_task_compile_shader->vertex_shader_source_=vertex_shader_text;
        render_task_compile_shader->fragment_shader_source_=fragment_shader_text;
        render_task_compile_shader->need_return_result=true;//需要返回结果
    }
    renderer->PushRenderTask(render_task_compile_shader);

    while (!glfwWindowShouldClose(window))
    {
        Render();

        //发出特殊任务：渲染结束
        RenderTaskFrameEnd* render_task_frame_end=new RenderTaskFrameEnd();
        renderer->PushRenderTask(render_task_frame_end);
        //等待渲染线程结束帧
        while(!render_task_frame_end->render_thread_end_frame_)
        {
        }

        //非渲染相关的API，例如处理系统事件，就放到主线程中。
        glfwPollEvents();
    }

    delete renderer;

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void Render(){
    if(program_id_==0){//等待Renderer线程编译Shader，并返回结果
        program_id_=render_task_compile_shader->result_program_id_;
        if(program_id_>0){
            delete render_task_compile_shader;//需要等待结果的渲染任务，需要在获取结果后删除。
        }else{
            return;
        }
    }

    //绘制任务
    RenderTaskDrawArray* render_task_draw_array=new RenderTaskDrawArray();
    {
        render_task_draw_array->render_command_=RenderCommand::DRAW_ARRAY;
        //构造参数
        render_task_draw_array->program_id_=program_id_;
        render_task_draw_array->positions_=kPositions;
        render_task_draw_array->positions_stride_=sizeof(glm::vec3);
        render_task_draw_array->colors_=kColors;
        render_task_draw_array->colors_stride_=sizeof(glm::vec4);
    }
    renderer->PushRenderTask(render_task_draw_array);
}