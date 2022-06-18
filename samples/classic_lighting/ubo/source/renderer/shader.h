//
// Created by captain on 2021/4/25.
//

#ifndef UNTITLED_SHADER_H
#define UNTITLED_SHADER_H

#include <string>
#include <unordered_map>
#include <pair>

using std::string;
using std::unordered_map;
using std::vector;
using std::pair;

class Shader {
public:
    Shader();
    ~Shader();

public:
    void Parse(string shader_name);//加载Shader文件并解析
    void CreateShaderProgram(const char* vertex_shader_text, const char* fragment_shader_text);//编译Shader,创建Shader程序;

    void Active();//激活
    void InActive();//禁用

    unsigned int shader_program_handle(){return shader_program_handle_;}//Shader程序句柄;

private:
    string shader_name_;//shader名
    unsigned int shader_program_handle_;//Shader程序句柄;

    unordered_map<string,unsigned int> uniform_block_binding_point_map_;//uniform black对应的binding point;

public:
    static Shader* Find(string shader_name);//查找或创建Shader

    static vector<pair<string,unsigned short>>& uniform_block_array(){
        return uniform_block_array_;
    }

private:
    static unordered_map<string,Shader*> kShaderMap;//已经创建的Shader

    static vector<pair<string,unsigned short>> uniform_block_array_;//所有Shader的uniform block统计。
};



#endif //UNTITLED_SHADER_H
