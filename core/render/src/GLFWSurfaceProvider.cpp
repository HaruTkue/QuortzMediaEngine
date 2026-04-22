#include "../include/vulkan_context.h"
#include "../include/GLFWSurfaceProvider.h"

VkSurfaceKHR GLFWSurfaceProvider::CreateSurface(VkInstance instance){
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, m_window, nullptr, &surface) != VK_SUCCESS){
        throw std::runtime_error("Failed to Create GLFW window surface");
    }

    return surface;
};
uint32_t GLFWSurfaceProvider::GetFramebufferWidth() const {
    int width;
    glfwGetFramebufferSize(m_window, &width , nullptr);
    return static_cast<uint32_t>(width);
}
uint32_t GLFWSurfaceProvider::GetFramebufferHeight() const {
    int height;
    glfwGetFramebufferSize(m_window, nullptr, &height);
    return static_cast<uint32_t>(height);
}