//
// Created by captainchen on 2022/2/7.
//

#include "renderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "VertexData.h"

Renderer::Renderer(GLFWwindow *window):window_(window), render_task_queue_(1024) {
    render_thread_ = std::thread(&Renderer::RenderMain, this);
    render_thread_.detach();
}

Renderer::~Renderer() {
    if (render_thread_.joinable()) {
        render_thread_.join();//等待渲染线程结束
    }
}

/// 编译、链接Shader
void CompileShader(RenderTaskBase* task_base){
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
    //设置返回结果
    task->result_program_id_=program;
}

/// 绘制
void DrawArray(RenderTaskBase* task_base, glm::mat4& projection, glm::mat4& view){
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

void Renderer::RenderMain() {
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

        if(render_task_queue_.empty()==false){
            RenderTaskBase* render_task = *(render_task_queue_.front());
            switch (render_task->render_command_) {//根据主线程发来的命令，做不同的处理
                case RenderCommand::COMPILE_SHADER:{
                    CompileShader(render_task);
                    break;
                }
                case RenderCommand::DRAW_ARRAY:
                    DrawArray(render_task, projection, view);
                    break;
            }
            render_task_queue_.pop();
            //如果这个任务不需要返回参数，那么用完就删掉。
            if(render_task->need_return_result==false){
                delete render_task;
            }
        }

        std::cout<<"task in queue:"<<render_task_queue_.size()<<std::endl;
        glfwSwapBuffers(window_);//这里有问题，每个Draw都执行了一次Swap？？
    }
}