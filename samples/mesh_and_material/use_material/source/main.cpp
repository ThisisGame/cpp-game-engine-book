#define GLFW_INCLUDE_NONE

#include <stdlib.h>
#include <stdio.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "utils/application.h"
#include "renderer/texture2d.h"
#include "renderer/mesh_filter.h"
#include "renderer/shader.h"
#include "renderer/material.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow* window;
GLint mvp_location, vpos_location, vcol_location,a_uv_location;
GLuint kVBO,kEBO;
GLuint kVAO;
MeshFilter* mesh_filter= nullptr;


//初始化OpenGL
void init_opengl()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

/// 创建VAO
void GeneratorVertexArrayObject(){
    glGenVertexArrays(1,&kVAO);
}

/// 创建VBO和EBO，设置VAO
void GeneratorBufferObject()
{
    //在GPU上创建缓冲区对象
    glGenBuffers(1,&kVBO);
    //将缓冲区对象指定为顶点缓冲区对象
    glBindBuffer(GL_ARRAY_BUFFER, kVBO);
    //上传顶点数据到缓冲区对象
    glBufferData(GL_ARRAY_BUFFER, mesh_filter->mesh()->vertex_num_ * sizeof(MeshFilter::Vertex), mesh_filter->mesh()->vertex_data_, GL_STATIC_DRAW);

    //在GPU上创建缓冲区对象
    glGenBuffers(1,&kEBO);
    //将缓冲区对象指定为顶点索引缓冲区对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kEBO);
    //上传顶点索引数据到缓冲区对象
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_filter->mesh()->vertex_index_num_ * sizeof(unsigned short), mesh_filter->mesh()->vertex_index_data_, GL_STATIC_DRAW);

    //设置VAO
    glBindVertexArray(kVAO);
    {
        //指定当前使用的VBO
        glBindBuffer(GL_ARRAY_BUFFER, kVBO);
        //将Shader变量(a_pos)和顶点坐标VBO句柄进行关联，最后的0表示数据偏移量。
        glVertexAttribPointer(vpos_location, 3, GL_FLOAT, false, sizeof(MeshFilter::Vertex), 0);
        //启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联
        glVertexAttribPointer(vcol_location, 4, GL_FLOAT, false, sizeof(MeshFilter::Vertex), (void*)(sizeof(float)*3));
        //将Shader变量(a_uv)和顶点UV坐标VBO句柄进行关联，最后的0表示数据偏移量。
        glVertexAttribPointer(a_uv_location, 2, GL_FLOAT, false, sizeof(MeshFilter::Vertex), (void*)(sizeof(float)*(3+4)));

        glEnableVertexAttribArray(vpos_location);
        glEnableVertexAttribArray(vcol_location);
        glEnableVertexAttribArray(a_uv_location);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kEBO);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int main(void)
{
    Application::set_data_path("../data/");
    init_opengl();

    mesh_filter=new MeshFilter();
    mesh_filter->LoadMesh("model/cube.mesh");

    Material* material=new Material();
    material->Parse("material/cube.mat");
    //获取`Shader`的`gl_program_id`，指定为目标Shader程序。
    GLuint gl_program_id=material->shader()->gl_program_id();

    mvp_location = glGetUniformLocation(gl_program_id, "u_mvp");
    vpos_location = glGetAttribLocation(gl_program_id, "a_pos");
    vcol_location = glGetAttribLocation(gl_program_id, "a_color");
    a_uv_location = glGetAttribLocation(gl_program_id, "a_uv");

    GeneratorVertexArrayObject();
    GeneratorBufferObject();

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glm::mat4 model,view, projection, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f/255,77.f/255,121.f/255,1.f);

        glm::mat4 trans = glm::translate(glm::vec3(0,0,0)); //不移动顶点坐标;

        static float rotate_eulerAngle=0.f;
        rotate_eulerAngle+=1;
        glm::mat4 rotation = glm::eulerAngleYXZ(glm::radians(rotate_eulerAngle), glm::radians(rotate_eulerAngle), glm::radians(rotate_eulerAngle)); //使用欧拉角旋转;

        glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f)); //缩放;
        model = trans*scale*rotation;

        view = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0,0), glm::vec3(0, 1, 0));

        projection=glm::perspective(glm::radians(60.f),ratio,1.f,1000.f);

        mvp=projection*view*model;

        //指定GPU程序(就是指定顶点着色器、片段着色器)
        glUseProgram(gl_program_id);
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);//开启背面剔除
            //上传mvp矩阵
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

            //拿到保存的Texture
            std::vector<std::pair<std::string,Texture2D*>> textures=material->textures();
            for (int texture_index = 0; texture_index < textures.size(); ++texture_index) {
                GLint u_texture_location= glGetUniformLocation(gl_program_id, textures[texture_index].first.c_str());
                //激活纹理单元0
                glActiveTexture(GL_TEXTURE0+texture_index);
                //将加载的图片纹理句柄，绑定到纹理单元0的Texture2D上。
                glBindTexture(GL_TEXTURE_2D,textures[texture_index].second->gl_texture_id());
                //设置Shader程序从纹理单元0读取颜色数据
                glUniform1i(u_texture_location,texture_index);
            }

            glBindVertexArray(kVAO);
            {
                glDrawElements(GL_TRIANGLES,mesh_filter->mesh()->vertex_index_num_,GL_UNSIGNED_SHORT,0);//使用顶点索引进行绘制，最后的0表示数据偏移量。
            }
            glBindVertexArray(0);
        }
        glUseProgram(-1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}