#define GLFW_INCLUDE_VULKAN
#include "../include/vulkan_context.h"

//winなので虫
//auto window = glfwCreateWindow(1280, 720 , "testwin" , nullptr, nullptr);

auto&VulkanCtx = VulkanContext::Get();