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

    //创建GPU程序
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
    //设置回传结果
    task->result_program_id_=program;
    task->return_result_set=true;
}

/// 绘制
/// \param task_base
/// \param projection
/// \param view
void RenderTaskConsumer::DrawArray(RenderTaskBase* task_base, glm::mat4& projection, glm::mat4& view){
    RenderTaskDrawArray* task= dynamic_cast<RenderTaskDrawArray*>(task_base);
    //坐标系变换
    glm::mat4 trans = glm::translate(glm::vec3(0,0,0)); //不移动顶点坐标;
    glm::mat4 rotation = glm::eulerAngleYXZ(glm::radians(0.f), glm::radians(0.f), glm::radians(0.f)); //使用欧拉角旋转;
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f)); //缩放;
    glm::mat4 model = trans*scale*rotation;

    glm::mat4 mvp=projection*view*model;

    //指定GPU程序(就是指定顶点着色器、片段着色器)
    glUseProgram(task->program_id_);
    //获取shader属性ID
    GLint mvp_location = glGetUniformLocation(task->program_id_, "u_mvp");
    GLint vpos_location = glGetAttribLocation(task->program_id_, "a_pos");
    GLint vcol_location = glGetAttribLocation(task->program_id_, "a_color");

    //启用顶点Shader属性(a_pos)，指定与顶点坐标数据进行关联
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, false, task->positions_stride_, task->positions_);

    //启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, false, task->colors_stride_, task->colors_);

    //上传mvp矩阵
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

    //上传顶点数据并进行绘制
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

/// 结束一帧
/// \param task_base
void RenderTaskConsumer::EndFrame(RenderTaskBase* task_base) {
    RenderTaskEndFrame *task = dynamic_cast<RenderTaskEndFrame *>(task_base);
    task->render_thread_frame_end_=true;
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
            switch (render_task->render_command_) {//根据主线程发来的命令，做不同的处理
                case RenderCommand::NONE:break;
                case RenderCommand::COMPILE_SHADER:{
                    CompileShader(render_task);
                    break;
                }
                case RenderCommand::DRAW_ARRAY:{
                    DrawArray(render_task, projection, view);
                    break;
                }
                case RenderCommand::END_FRAME:break;
            }
            RenderTaskQueue::Pop();
            //如果这个任务不需要返回参数，那么用完就删掉。
            if(render_task->need_return_result==false){
                delete render_task;
            }

            //如果是帧结束任务，就交换缓冲区。
            if(render_task->render_command_==RenderCommand::END_FRAME){
                EndFrame(render_task);
                glfwSwapBuffers(window_);
                break;
            }
        }
        std::cout<<"task in queue:"<<RenderTaskQueue::Size()<<std::endl;
    }
}