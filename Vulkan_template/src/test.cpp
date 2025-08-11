// #define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
// #include "funcs/func.h"

/*
main.cpp 中包含 GLEW 的头文件之前定义 GLEW_STATIC 
*/
int main() {

    std::cout << "my openGL test 1" << std::endl;
     


    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 创建一个窗口和 OpenGL 上下文
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW and GLEW Test", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化 GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // 设置窗口关闭回调
    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(0);
        });

    // 设置窗口大小改变回调
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        });

    // 循环直到用户关闭窗口
    while (!glfwWindowShouldClose(window)) {
        // 渲染逻辑
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 交换缓冲区和轮询事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 退出清理
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}