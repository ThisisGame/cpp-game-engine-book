#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include <stdio.h>

#pragma region 顶点坐标 顶点颜色

struct Vector3
{
    float x,y,z;
};

struct Color
{
    float r, g, b;
};

static const Vector3 Positions[36] =
{
        //前
        { -1.0f, -1.0f,1.0f},//左下
        { 1.0f,  -1.0f,1.0f},//右下
        { 1.0f,  1.0f,1.0f},//右上
        {  1.0f, 1.0f,1.0f},//右上
        { -1.0f, -1.0f,1.0f},//左上
        { -1.0f, 1.0f,1.0f},//左下

        //后
        { -1.0f, -1.0f,-1.0f},//左下
        {  1.0f, -1.0f,-1.0f},//右下
        {   1.0f,  1.0f,-1.0f},//右上
        {   1.0f,  1.0f,-1.0f},//右上
        { -1.0f, -1.0f,-1.0f},//左上
        { -1.0f,1.0f,-1.0f},//左下

        //左
        { -1.0f, -1.0f,-1.0f},//左下
        {  -1.0f, -1.0f,1.0f},//右下
        {   -1.0f,  1.0f,1.0f},//右上

        {   -1.0f,  1.0f,1.0f},//右上
        { -1.0f, 1.0f,-1.0f},//左上
        { -1.0f, -1.0f,-1.0f},//左下

        //右
        { 1.0f, -1.0f,-1.0f},//左下
        { 1.0f, -1.0f,1.0f},//右下
        { 1.0f, 1.0f,1.0f},//右上

        { 1.0f, 1.0f,1.0f},//右上
        { 1.0f, 1.0f,-1.0f},//左上
        { 1.0f, -1.0f,-1.0f},//左下

        //上
        { -1.0f, 1.0f,1.0f},//左下
        { 1.0f,  1.0f,1.0f},//右下
        { 1.0f,  1.0f,-1.0f},//右上

        { 1.0f,  1.0f,-1.0f},//右上
        { -1.0f, 1.0f,-1.0f},//左下
        { -1.0f, 1.0f,1.0f},//左下

        //下
        { -1.0f, -1.0f,1.0f},//左下
        { 1.0f,  -1.0f,1.0f},//右下
        { 1.0f,  -1.0f,-1.0f},//右上

        { 1.0f,  -1.0f,-1.0f},//右上
        { -1.0f, -1.0f,-1.0f},//左下
        { -1.0f, -1.0f,1.0f},//左下
};

static const Color Colors[36] =
{
        //前
        { 1.f, 0.f, 0.f },//左下
        { 1.f, 0.f, 0.f },//右下
        { 1.f, 0.f, 0.f },//右上

        { 1.f, 0.f, 0.f },//右上
        { 1.f, 0.f, 0.f },//左上
        { 1.f, 0.f, 0.f },//左下

        //后
        { 0.f, 1.f, 0.f },//左下
        { 0.f, 1.f, 0.f },//右下
        { 0.f, 1.f, 0.f },//右上

        { 0.f, 1.f, 0.f },//右上
        { 0.f, 1.f, 0.f },//左上
        { 0.f, 1.f, 0.f },//左下

        //左
        { 0.f, 0.f, 1.f },//左下
        { 0.f, 0.f, 1.f },//右下
        { 0.f, 0.f, 1.f },//右上

        { 0.f, 0.f, 1.f },//右上
        { 0.f, 0.f, 1.f },//左上
        { 0.f, 0.f, 1.f},//左下

        //右
        { 1.f, 1.f, 0.f },//左下
        { 1.f, 1.f, 0.f },//右下
        { 1.f, 1.f, 0.f },//右上

        { 1.f, 1.f, 0.f },//右上
        { 1.f, 1.f, 0.f },//左上
        { 1.f, 1.f, 0.f },//左下

        //上
        { 0.f, 1.f, 1.f },//左下
        { 0.f, 1.f, 1.f },//右下
        { 0.f, 1.f, 1.f },//右上

        { 0.f, 1.f, 1.f },//右上
        { 0.f, 1.f, 1.f },//左上
        { 0.f, 1.f, 1.f },//左下

        //下
        { 1.f, 0.f, 1.f },//左下
        { 1.f, 0.f, 1.f },//右下
        { 1.f, 0.f, 1.f },//右上

        { 1.f, 0.f, 1.f },//右上
        { 1.f, 0.f, 1.f },//左上
        { 1.f, 0.f, 1.f},//左下
};

#pragma  endregion 顶点坐标 顶点颜色

#pragma region 顶点Shader和片段Shader

static const char* vertex_shader_text =
        "#version 110\n"
        "uniform mat4 MVP;\n"
        "attribute vec3 vCol;\n"
        "attribute vec3 vPos;\n"
        "varying vec3 color;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = MVP * vec4(vPos, 1.0);\n"
        "    color = vCol;\n"
        "}\n";

static const char* fragment_shader_text =
        "#version 110\n"
        "varying vec3 color;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(color, 1.0);\n"
        "}\n";

#pragma endregion

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow* window;
GLuint vertex_shader, fragment_shader, program;
GLint mvp_location, vpos_location, vcol_location;

//初始化OpenGL
void init_opengl()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(960, 640, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
}

//编译、链接Shader
void compile_shader()
{
    //创建顶点Shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    //指定Shader源码
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    //编译Shader
    glCompileShader(vertex_shader);

    //创建片段Shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //指定Shader源码
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    //编译Shader
    glCompileShader(fragment_shader);

    //创建GPU程序
    program = glCreateProgram();
    //附加Shader
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    //Link
    glLinkProgram(program);
}

int main(void)
{
    init_opengl();

    compile_shader();

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glEnableVertexAttribArray(vcol_location);


    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m,v, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime());//立方体绕Z轴旋转，导致只有朝向摄像机的一面被看到。
        mat4x4_rotate_Y(m, m, (float) glfwGetTime());
        mat4x4_rotate_X(m, m, (float) glfwGetTime());

        vec3 eye={0,0,10};
        vec3 center={0,0,0};
        vec3 up={0,1,0};
        mat4x4_look_at(v,eye,center,up);

        mat4x4_ortho(p, -3, 3, -2.f, 2.f, -10.f, 10.f);

        mat4x4_mul(mvp, p, m);
//        mat4x4_mul(mvp, mvp, v);
//        mat4x4_mul(mvp, mvp, p);

        //指定GPU程序(就是指定顶点着色器、片段着色器)
        glUseProgram(program);
            glEnable(GL_DEPTH_TEST);
            //上传顶点坐标数据
            glVertexAttribPointer(vpos_location, 3, GL_FLOAT, false, sizeof(Vector3), Positions);
            //上传顶点颜色数据
            glVertexAttribPointer(vcol_location, 3, GL_FLOAT, false, sizeof(Color), Colors);
            //上传mvp矩阵
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

            //void glDrawArrays(GLenum mode,GLint first,GLsizei count);
            glDrawArrays(GL_TRIANGLES, 0, 6*6);//表示从第0个顶点开始画，总共画6个面，每个面6个顶点。

        glUseProgram(-1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}