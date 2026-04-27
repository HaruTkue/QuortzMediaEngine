#include "../include/vulkan_context.h"
#include "../include/swapchain.h"
#include "../include/draw.h"
#include "../include/surface_provider.h"

#include <thread>
#include <chrono>

void Draw::OnDrawFrame(){
    auto& vulkanCtx = VulkanContext::Get();
    auto& Swapchain = vulkanCtx.GetSwapchain();
    auto device = vulkanCtx.GetVkDevice();

    if (vulkanCtx.AcquireNextImage() != VK_SUCCESS){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return;
    }
    auto* frameCtx = vulkanCtx.GetCurrentFrameContext();
    auto& commandBuffer = frameCtx->commandBuffer;
    commandBuffer->Begin();
    //描画コード (仮置き)
    VkImageSubresourceRange range{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0 , .levelCount=  1,
        .baseArrayLayer =0 , .layerCount = 1,
    };
    commandBuffer->TransitionLayout(
        Swapchain->GetCurrentImage(), range,
        ImageLayoutTransition::FromUndefinedToColorAttachment()
    );

    auto imageView = Swapchain->GetCurrentImage();
    auto extent = Swapchain->GetExtent();

    VkRenderingAttachmentI
    //Layout 表示
    commandBuffer->TransitionLayout(
        Swapchain->GetCurrentImage(), range,
        ImageLayoutTransition::FromColorToPresent()
    );
    commandBuffer->End();
    vulkanCtx.SubmitPresent();
}