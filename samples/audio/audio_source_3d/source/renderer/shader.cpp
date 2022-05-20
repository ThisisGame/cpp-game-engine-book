//
// Created by captain on 2021/4/25.
//

#include "shader.h"
#include <iostream>
#include <fstream>
#include <glad/gl.h>
#include "utils/debug.h"
#include "utils/application.h"

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;
using std::pair;

unordered_map<string,Shader*> Shader::kShaderMap;

Shader::Shader() {

}

Shader::~Shader() {

}

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

    //组装完整文件路径
    string vertex_shader_file_path=Application::data_path()+shader_name+".vs";
    string fragment_shader_file_path=Application::data_path()+shader_name+".fs";

    //读取顶点Shader代码
    ifstream vertex_shader_input_file_stream(vertex_shader_file_path);
    string vertex_shader_source((std::istreambuf_iterator<char>(vertex_shader_input_file_stream)),std::istreambuf_iterator<char>());
    //读取片段Shader代码
    ifstream fragment_shader_input_file_stream(fragment_shader_file_path);
    string fragment_shader_source((std::istreambuf_iterator<char>(fragment_shader_input_file_stream)),std::istreambuf_iterator<char>());

    CreateGPUProgram(vertex_shader_source.c_str(), fragment_shader_source.c_str());
}

void Shader::CreateGPUProgram(const char* vertex_shader_text, const char* fragment_shader_text) {
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
    }

    //创建GPU程序
    gl_program_id_ = glCreateProgram();
    //附加Shader
    glAttachShader(gl_program_id_, vertex_shader);
    glAttachShader(gl_program_id_, fragment_shader);
    //Link
    glLinkProgram(gl_program_id_);
    //获取编译结果
    GLint link_status=GL_FALSE;
    glGetProgramiv(gl_program_id_, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE)
    {
        GLchar message[256];
        glGetProgramInfoLog(gl_program_id_, sizeof(message), 0, message);
        DEBUG_LOG_ERROR("link shader error:{}",message);
    }
}

void Shader::Active() {
    glUseProgram(gl_program_id_);
}

void Shader::InActive() {
    
}

