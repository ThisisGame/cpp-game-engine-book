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
#include "renderer/mesh_filter.h"
#include "renderer/material.h"
#include "renderer/mesh_renderer.h"
#include "renderer/camera.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow* window;

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

int main(void)
{
    Application::set_data_path("../data/");
    init_opengl();

    MeshFilter* mesh_filter=new MeshFilter();
    mesh_filter->LoadMesh("model/plane.008.mesh");

    Material* material=new Material();
    material->Parse("material/plane.008.mat");

    MeshRenderer* mesh_renderer=new MeshRenderer();
    mesh_renderer->SetMeshFilter(mesh_filter);
    mesh_renderer->SetMaterial(material);

    Camera* camera=new Camera();
    camera->set_clear_color(49.f/255,77.f/255,121.f/255,1.f);
    camera->set_clear_flag(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    while (!glfwWindowShouldClose(window))
    {
        camera->Clear();

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        ratio = width / (float) height;

        camera->SetView(glm::vec3(0, 0, 10), glm::vec3(0, 0,0), glm::vec3(0, 1, 0));
        camera->SetProjection(60.f,ratio,1.f,1000.f);

        glm::mat4 trans = glm::translate(glm::vec3(0,0,0)); //不移动顶点坐标;
        static float rotate_eulerAngle=0.f;
        rotate_eulerAngle+=0.1f;
        glm::mat4 rotation = glm::eulerAngleYXZ(glm::radians(rotate_eulerAngle), glm::radians(rotate_eulerAngle), glm::radians(rotate_eulerAngle)); //使用欧拉角旋转;

        glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)); //缩放;
        glm::mat4 model = trans*scale*rotation;
        glm::mat4 mvp=camera->projection_mat4()*camera->view_mat4()*model;

        mesh_renderer->SetMVP(mvp);
        mesh_renderer->Render();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}