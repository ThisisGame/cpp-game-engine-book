//
// Created by captain on 2021/4/25.
//

#include "shader.h"
#include <iostream>
#include <fstream>
#include <glad/gl.h>
#include "utils/debug.h"
#include "app/application.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;
using std::pair;

unordered_map<string,Shader*> Shader::kShaderMap;
vector<pair<string,unsigned short>> Shader::uniform_block_array_={{"Ambient",16},{"Light",28}};

Shader::Shader()=default;

Shader::~Shader()=default;

Shader* Shader::Find(string shader_name) {
    unordered_map<string,Shader*>::iterator iter=kShaderMap.find(shader_name);
    if(iter!=kShaderMap.end()) {
        return iter->second;
    }

    Shader* shader=new Shader();
    shader->Parse(shader_name);

    kShaderMap.insert(pair<string,Shader*>(shader_name,shader));

    return shader;
}


void Shader::Parse(string shader_name) {
    shader_name_=shader_name;

    // 组装完整文件路径
    string vertex_shader_file_path=Application::data_path()+shader_name+".vert";
    string fragment_shader_file_path=Application::data_path()+shader_name+".frag";

    // 读取顶点Shader代码
    ifstream vertex_shader_input_file_stream(vertex_shader_file_path);
    string vertex_shader_source((std::istreambuf_iterator<char>(vertex_shader_input_file_stream)),std::istreambuf_iterator<char>());
    // 读取片段Shader代码
    ifstream fragment_shader_input_file_stream(fragment_shader_file_path);
    string fragment_shader_source((std::istreambuf_iterator<char>(fragment_shader_input_file_stream)),std::istreambuf_iterator<char>());

    CreateShaderProgram(vertex_shader_source.c_str(), fragment_shader_source.c_str());
    ConnectUniformBlockAndBindingPoint();
}

void Shader::CreateShaderProgram(const char* vertex_shader_text, const char* fragment_shader_text) {
    //编译Shader任务
    shader_program_handle_=GPUResourceMapper::GenerateShaderProgramHandle();
    RenderTaskProducer::ProduceRenderTaskCompileShader(vertex_shader_text, fragment_shader_text, shader_program_handle_);
}

void Shader::ConnectUniformBlockAndBindingPoint() {
    for (int i = 0; i < uniform_block_array_.size(); ++i) {
        string uniform_block_name=uniform_block_array_[i].first;
        GLuint uniform_block_index = glGetUniformBlockIndex(shader_program, uniform_block_name);
        GLuint uniform_block_binding_point=i;
        glUniformBlockBinding(shader_program, uniform_block_index, uniform_block_binding_point);
        uniform_block_binding_point_map_[uniform_block_name]=uniform_block_binding_point;
    }
}

void Shader::Active() {
    //使用Shader程序任务
    RenderTaskProducer::ProduceRenderTaskUseShaderProgram(shader_program_handle_);
}

void Shader::InActive() {
    
}

