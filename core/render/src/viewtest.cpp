#define GLFW_INCLUDE_VULKAN
#include "../include/vulkan_context.h"
#include <GLFW/glfw3.h>

//winなので虫
auto window = glfwCreateWindow(1280, 720 , "testwin" , nullptr, nullptr);
