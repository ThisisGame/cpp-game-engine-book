//
// Created by captain on 2021/4/25.
//

#ifndef UNTITLED_SHADER_H
#define UNTITLED_SHADER_H

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

class Shader {
public:
    Shader();
    ~Shader();

public:
    void Parse(string shader_name);//加载Shader文件并解析
    void CreateGPUProgram(const char* vertex_shader_text, const char* fragment_shader_text);//编译Shader,创建GPU程序;

    unsigned int gl_program_id(){return gl_program_id_;}//glCreateProgram()返回的GPU程序句柄;

private:
    string shader_name_;//shader名
    unsigned int gl_program_id_;//glCreateProgram()返回的GPU程序句柄;


public:
    static Shader* Find(string shader_name);//查找或创建Shader
private:
    static unordered_map<string,Shader*> kShaderMap;//已经创建的Shader
};



#endif //UNTITLED_SHADER_H
