#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include "rasterizer/Scene.hpp"
// simple wrap of GLFW
template<int width, int height>
class Window {
   private:
    GLFWwindow* _window_handle;
    Rasterizer::Scene<width, height>& _scene;

   public:
    std::string title;
    static bool init_flag;
    Window(const std::string& title)
        :title{title} {
        if (!Window::init_flag) {
            if (!glfwInit()) {
                std::cerr << "GLFW init failed!";
                exit(1);
            }
            Window::init_flag = true;
        }
        _window_handle =
            glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    }
    void setScene(Rasterizer::Scene<width, height>& scene){
        this->_scene = scene;
    }
    // this leads to a loop!
    void show() {
        glfwMakeContextCurrent(_window_handle);
        while (!glfwWindowShouldClose(_window_handle)) {
            glfwPollEvents();
            this->_scene.render();       
        }
    }
    ~Window() {
        glfwDestroyWindow(_window_handle);
        glfwTerminate();
        Window::init_flag = true;
    }
};

#endif /* WINDOW_HPP */
