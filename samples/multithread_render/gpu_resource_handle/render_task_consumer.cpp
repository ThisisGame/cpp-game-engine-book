//
// Created by captainchen on 2022/2/7.
//

#include "render_task_consumer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "VertexData.h"
#include "render_task_type.h"
#include "render_command.h"
#include "render_task_queue.h"
#include "gpu_resource_mapper.h"

GLFWwindow* RenderTaskConsumer::window_;
std::thread RenderTaskConsumer::render_thread_;//渲染线程

void RenderTaskConsumer::Init(GLFWwindow *window) {
    window_ = window;
    render_thread_ = std::thread(&RenderTaskConsumer::ProcessTask);
    render_thread_.detach();
}

void RenderTaskConsumer::Exit() {
    if (render_thread_.joinable()) {
        render_thread_.join();//等待渲染线程结束
    }
}

/// 编译、链接Shader
/// \param task_base
void RenderTaskConsumer::CompileShader(RenderTaskBase* task_base){
    RenderTaskCompileShader* task= dynamic_cast<RenderTaskCompileShader*>(task_base);
    const char* vertex_shader_text=task->vertex_shader_source_;
    const char* fragment_shader_text=task->fragment_shader_source_;
    //创建顶点Shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    //指定Shader源码
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    //编译Shader
    glCompileShader(vertex_shader);
    //获取编译结果
    GLint compile_status=GL_FALSE;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE)
    {
        GLchar message[256];
        glGetShaderInfoLog(vertex_shader, sizeof(message), 0, message);
        std::cout<<"compile vs error:"<<message<<std::endl;
    }

    //创建片段Shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //指定Shader源码
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    //编译Shader
    glCompileShader(fragment_shader);
    //获取编译结果
    compile_status=GL_FALSE;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE)
    {
        GLchar message[256];
        glGetShaderInfoLog(fragment_shader, sizeof(message), 0, message);
        std::cout<<"compile fs error:"<<message<<std::endl;
    }

    //创建Shader程序
    GLuint program = glCreateProgram();
    //附加Shader
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    //Link
    glLinkProgram(program);
    //获取编译结果
    GLint link_status=GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE)
    {
        GLchar message[256];
        glGetProgramInfoLog(program, sizeof(message), 0, message);
        std::cout<<"link error:"<<message<<std::endl;
    }
    //将主线程中产生的Shader程序句柄 映射到 Shader程序
    GPUResourceMapper::MapShaderProgram(task->shader_program_handle_, program);
}

/// 创建VAO
/// \param task_base
void RenderTaskConsumer::CreateVAO(RenderTaskBase* task_base){
    RenderTaskCreateVAO* task= dynamic_cast<RenderTaskCreateVAO*>(task_base);
    //从映射表中，获取Shader程序句柄
    GLuint shader_program= GPUResourceMapper::GetShaderProgram(task->shader_program_handle_);
    GLuint vbo_pos,vbo_color,vao;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo_pos);
    glGenBuffers(1, &vbo_color);

    GLint attribute_pos_location = glGetAttribLocation(shader_program, "a_pos");
    GLint attribute_col_location = glGetAttribLocation(shader_program, "a_color");

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kPositions), kPositions, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute_pos_location, 3, GL_FLOAT, false, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(attribute_pos_location);//启用顶点Shader属性(a_pos)，指定与顶点坐标数据进行关联

    glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kColors), kColors, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute_col_location, 3, GL_FLOAT, false, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(attribute_col_location);//启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //将主线程中产生的VAO句柄 映射到 VAO
    GPUResourceMapper::MapVAO(task->vao_handle_,vao);
}

/// 绘制
/// \param task_base
/// \param projection
/// \param view
void RenderTaskConsumer::DrawArray(RenderTaskBase* task_base, glm::mat4& projection, glm::mat4& view){
    RenderTaskDrawArray* task= dynamic_cast<RenderTaskDrawArray*>(task_base);

    //从映射表中，获取Shader程序句柄
    GLuint shader_program= GPUResourceMapper::GetShaderProgram(task->shader_program_handle_);
    //从映射表中，获取VAO
    GLuint vao= GPUResourceMapper::GetVAO(task->vao_handle_);

    //指定Shader程序
    glUseProgram(shader_program);
    {
        glBindVertexArray(vao);

        //上传顶点数据并进行绘制
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

/// 结束一帧
/// \param task_base
void RenderTaskConsumer::EndFrame(RenderTaskBase* task_base) {
    RenderTaskEndFrame *task = dynamic_cast<RenderTaskEndFrame *>(task_base);
    glfwSwapBuffers(window_);
    task->return_result_set=true;
}

void RenderTaskConsumer::ProcessTask() {
    //渲染相关的API调用需要放到渲染线程中。
    glfwMakeContextCurrent(window_);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window_))
    {
        float ratio;
        int width, height;
        glm::mat4 model,view, projection, mvp;

        //获取画面宽高
        glfwGetFramebufferSize(window_, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f/255,77.f/255,121.f/255,1.f);

        view = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0,0), glm::vec3(0, 1, 0));

        projection=glm::perspective(glm::radians(60.f),ratio,1.f,1000.f);

        while(true){
            if(RenderTaskQueue::Empty()){//渲染线程一直等待主线程发出任务。没有了任务Sleep 1微秒。
                std::this_thread::sleep_for(std::chrono::microseconds(1));
                continue;
            }
            RenderTaskBase* render_task = RenderTaskQueue::Front();
            RenderCommand render_command=render_task->render_command_;
            switch (render_command) {//根据主线程发来的命令，做不同的处理
                case RenderCommand::NONE:break;
                case RenderCommand::COMPILE_SHADER:{
                    CompileShader(render_task);
                    break;
                }
                case RenderCommand::CREATE_VAO:{
                    CreateVAO(render_task);
                    break;
                }
                case RenderCommand::DRAW_ARRAY:{
                    DrawArray(render_task, projection, view);
                    break;
                }
                case RenderCommand::END_FRAME:{
                    EndFrame(render_task);
                    break;
                }
            }
            RenderTaskQueue::Pop();
            //如果这个任务不需要返回参数，那么用完就删掉。
            if(render_task->need_return_result==false){
                delete render_task;
            }

            //如果是帧结束任务，就交换缓冲区。
            if(render_command==RenderCommand::END_FRAME){
                break;
            }
        }
        std::cout<<"task in queue:"<<RenderTaskQueue::Size()<<std::endl;
    }
}