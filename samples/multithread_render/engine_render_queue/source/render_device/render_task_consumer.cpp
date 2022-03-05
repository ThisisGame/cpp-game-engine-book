//
// Created by captainchen on 2022/2/7.
//

#include "render_task_consumer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "VertexData.h"
#include "utils/debug.h"
#include "render_task_type.h"
#include "render_command.h"
#include "gpu_resource_mapper.h"
#include "utils/screen.h"

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

void RenderTaskConsumer::PushRenderTask(RenderTaskBase* render_task) {
    render_task_queue_.push(render_task);
}

/// 更新游戏画面尺寸
void RenderTaskConsumer::UpdateScreenSize(RenderTaskBase* task_base) {
    RenderTaskUpdateScreenSize* task= dynamic_cast<RenderTaskUpdateScreenSize*>(task_base);
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);
    Screen::set_width_height(width,height);
}

/// 编译、链接Shader
/// \param task_base
void RenderTaskConsumer::CompileShader(RenderTaskBase* task_base){
    RenderTaskCompileShader* task= dynamic_cast<RenderTaskCompileShader*>(task_base);
    const char* vertex_shader_text=task->vertex_shader_source_;
    const char* fragment_shader_text=task->fragment_shader_source_;

    //创建顶点Shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
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
        DEBUG_LOG_ERROR("compile vertex shader error:{}",message);
        return;
    }

    //创建片段Shader
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
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
        DEBUG_LOG_ERROR("compile fragment shader error:{}",message);
        return;
    }

    //创建Shader程序
    GLuint shader_program = glCreateProgram();
    //附加Shader
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    //Link
    glLinkProgram(shader_program);
    //获取编译结果
    GLint link_status=GL_FALSE;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE)
    {
        GLchar message[256];
        glGetProgramInfoLog(shader_program, sizeof(message), 0, message);
        DEBUG_LOG_ERROR("link shader error:{}",message);
        return;
    }
    //将主线程中产生的Shader程序句柄 映射到 Shader程序
    GPUResourceMapper::MapShaderProgram(task->shader_program_handle_, shader_program);
}

void RenderTaskConsumer::UseShaderProgram(RenderTaskBase *task_base) {
    RenderTaskUseShaderProgram* task= dynamic_cast<RenderTaskUseShaderProgram*>(task_base);
    GLuint shader_program = GPUResourceMapper::GetShaderProgram(task->shader_program_handle_);
    glUseProgram(shader_program);
}

void RenderTaskConsumer::CreateCompressedTexImage2D(RenderTaskBase *task_base) {
    RenderTaskCreateCompressedTexImage2D* task= dynamic_cast<RenderTaskCreateCompressedTexImage2D*>(task_base);

    GLuint texture_id;

    //1. 通知显卡创建纹理对象，返回句柄;
    glGenTextures(1, &texture_id);__CHECK_GL_ERROR__

    //2. 将纹理绑定到特定纹理目标;
    glBindTexture(GL_TEXTURE_2D, texture_id);__CHECK_GL_ERROR__

    //3. 将压缩纹理数据上传到GPU;
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, task->texture_format_, task->width_, task->height_, 0, task->compress_size_, task->data_);
    __CHECK_GL_ERROR__

    //4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);__CHECK_GL_ERROR__
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);__CHECK_GL_ERROR__

    //将主线程中产生的压缩纹理句柄 映射到 纹理
    GPUResourceMapper::MapTexture(task->texture_handle_, texture_id);
}

void RenderTaskConsumer::CreateTexImage2D(RenderTaskBase *task_base) {
    RenderTaskCreateTexImage2D* task= dynamic_cast<RenderTaskCreateTexImage2D*>(task_base);

    GLuint texture_id;

    //1. 通知显卡创建纹理对象，返回句柄;
    glGenTextures(1, &texture_id);__CHECK_GL_ERROR__

    //2. 将纹理绑定到特定纹理目标;
    glBindTexture(GL_TEXTURE_2D, texture_id);__CHECK_GL_ERROR__

    //3. 将图片rgb数据上传到GPU;
    glTexImage2D(GL_TEXTURE_2D, 0, task->gl_texture_format_, task->width_, task->height_, 0, task->client_format_, task->data_type_, task->data_);__CHECK_GL_ERROR__

    //4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);__CHECK_GL_ERROR__
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);__CHECK_GL_ERROR__

    //将主线程中产生的纹理句柄 映射到 纹理
    GPUResourceMapper::MapTexture(task->texture_handle_, texture_id);
}

/// 删除Textures
/// \param task_base
void RenderTaskConsumer::DeleteTextures(RenderTaskBase *task_base) {
    RenderTaskDeleteTextures* task= dynamic_cast<RenderTaskDeleteTextures*>(task_base);
    //从句柄转换到纹理对象
    GLuint* texture_id_array=new GLuint[task->texture_count_];
    for (int i = 0; i < task->texture_count_; ++i) {
        texture_id_array[i]=GPUResourceMapper::GetTexture(task->texture_handle_array_[i]);
    }
    glDeleteTextures(task->texture_count_,texture_id_array);__CHECK_GL_ERROR__
    delete [] texture_id_array;
}

/// 局部更新纹理
/// \param task_base
void RenderTaskConsumer::UpdateTextureSubImage2D(RenderTaskBase *task_base) {
    RenderTaskUpdateTextureSubImage2D* task= dynamic_cast<RenderTaskUpdateTextureSubImage2D*>(task_base);
    GLuint texture=GPUResourceMapper::GetTexture(task->texture_handle_);
    glBindTexture(GL_TEXTURE_2D, texture);__CHECK_GL_ERROR__
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);__CHECK_GL_ERROR__
    glTexSubImage2D(GL_TEXTURE_2D,0,task->x_,task->y_,task->width_,task->height_,task->client_format_,task->data_type_,task->data_);__CHECK_GL_ERROR__
}

void RenderTaskConsumer::CreateVAO(RenderTaskBase *task_base) {
    RenderTaskCreateVAO* task=dynamic_cast<RenderTaskCreateVAO*>(task_base);
    GLuint shader_program=GPUResourceMapper::GetShaderProgram(task->shader_program_handle_);
    GLint attribute_pos_location = glGetAttribLocation(shader_program, "a_pos");
    GLint attribute_color_location = glGetAttribLocation(shader_program, "a_color");
    GLint attribute_uv_location = glGetAttribLocation(shader_program, "a_uv");

    GLuint vertex_buffer_object,element_buffer_object,vertex_array_object;
    //在GPU上创建缓冲区对象
    glGenBuffers(1,&vertex_buffer_object);
    //将缓冲区对象指定为顶点缓冲区对象
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    //上传顶点数据到缓冲区对象
    glBufferData(GL_ARRAY_BUFFER, task->vertex_data_size_, task->vertex_data_, GL_DYNAMIC_DRAW);

    //在GPU上创建缓冲区对象
    glGenBuffers(1,&element_buffer_object);
    //将缓冲区对象指定为顶点索引缓冲区对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
    //上传顶点索引数据到缓冲区对象
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, task->vertex_index_data_size_, task->vertex_index_data_, GL_STATIC_DRAW);

    glGenVertexArrays(1,&vertex_array_object);

    //设置VAO
    glBindVertexArray(vertex_array_object);__CHECK_GL_ERROR__
    {
        //指定当前使用的VBO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);__CHECK_GL_ERROR__
        //将Shader变量(a_pos)和顶点坐标VBO句柄进行关联，最后的0表示数据偏移量。
        glVertexAttribPointer(attribute_pos_location, 3, GL_FLOAT, false, task->vertex_data_stride_, 0);__CHECK_GL_ERROR__
        //启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联
        glVertexAttribPointer(attribute_color_location, 4, GL_FLOAT, false, task->vertex_data_stride_, (void*)(sizeof(float) * 3));__CHECK_GL_ERROR__
        //将Shader变量(a_uv)和顶点UV坐标VBO句柄进行关联，最后的0表示数据偏移量。
        glVertexAttribPointer(attribute_uv_location, 2, GL_FLOAT, false, task->vertex_data_stride_, (void*)(sizeof(float) * (3 + 4)));__CHECK_GL_ERROR__

        glEnableVertexAttribArray(attribute_pos_location);__CHECK_GL_ERROR__
        glEnableVertexAttribArray(attribute_color_location);__CHECK_GL_ERROR__
        glEnableVertexAttribArray(attribute_uv_location);__CHECK_GL_ERROR__

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);__CHECK_GL_ERROR__
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);__CHECK_GL_ERROR__
    //将主线程中产生的VAO句柄 映射到 VAO
    GPUResourceMapper::MapTexture(task->vao_handle_, vertex_array_object);
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
            if(render_task_queue_.empty()){//渲染线程一直等待主线程发出任务。
                continue;
            }
            RenderTaskBase* render_task = *(render_task_queue_.front());
            switch (render_task->render_command_) {//根据主线程发来的命令，做不同的处理
                case RenderCommand::NONE:break;
                case RenderCommand::UPDATE_SCREEN_SIZE:{
                    UpdateScreenSize(render_task);
                    break;
                }
                case RenderCommand::COMPILE_SHADER:{
                    CompileShader(render_task);
                    break;
                }
                case RenderCommand::USE_SHADER_PROGRAM:{
                    break;
                }
                case RenderCommand::CREATE_COMPRESSED_TEX_IMAGE2D:{
                    CreateCompressedTexImage2D(render_task);
                    break;
                }
                case RenderCommand::CREATE_TEX_IMAGE2D:{
                    CreateTexImage2D(render_task);
                    break;
                }
                case RenderCommand::DELETE_TEXTURES:{
                    DeleteTextures(render_task);
                    break;
                }
                case RenderCommand::UPDATE_TEXTURE_SUB_IMAGE2D:{
                    UpdateTextureSubImage2D(render_task);
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
                case RenderCommand::END_FRAME:break;
            }
            render_task_queue_.pop();
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
        std::cout<<"task in queue:"<<render_task_queue_.size()<<std::endl;
    }
}