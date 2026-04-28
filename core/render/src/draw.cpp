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

    auto imageView = Swapchain->GetCurrentView();
    auto extent = Swapchain->GetExtent();

    VkRenderingAttachmentInfo colorAttachment{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = imageView,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp= VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = VkClearValue{ .color = {{0.6f , 0.2f, 0.3f , 1.0f}}}
    };
    VkRenderingInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {{0,0} , extent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment
    };
    vkCmdBeginRendering(*commandBuffer,  &renderingInfo);
    vkCmdEndRendering(*commandBuffer);

    //Layout 表示
    commandBuffer->TransitionLayout(
        Swapchain->GetCurrentImage(), range,
        ImageLayoutTransition::FromColorToPresent()
    );
    commandBuffer->End();
    vulkanCtx.SubmitPresent();
}

void Draw::OnInitilize(){
    // 初期起動
    InitializeVertexBuffer();
    InitializeGraphicsPipeline();
}
void Draw::OnCleanup(){
    auto& vulkanCtx = VulkanContext::Get();
    auto device = vulkanCtx.GetVkDevice();

    //GPUがidleになるまで
    vkDeviceWaitIdle(device);
}