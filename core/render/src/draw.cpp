#include "../include/vulkan_context.h"
#include "../include/swapchain.h"
#include "../include/draw.h"
#include <thread>
#include <chrono>

void Draw::OnDrawFrame(){
    auto& vulkanCtx = VulkanContext::Get();
    auto& Swapchain = vulkanCtx.GetSwapchain();
    auto device = vulkanCtx.GetVkDevice();

    if (vulkanCtx.AcquireNextImage() != VK_SUCCESS){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}