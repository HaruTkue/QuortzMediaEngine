#include "../include/command_buffer.h"

CommandBuffer::CommandBuffer(VkCommandBuffer CommandBuffer){
    m_commandBuffer = CommandBuffer;
}

CommandBuffer::~CommandBuffer(){
    auto& vulkanCtx = VulkanContext::Get();
    vkFreeCommandBuffers(vulkanCtx.GetVkDevice(), vulkanCtx.GetCommandPool(), 1 ,&m_commandBuffer);
    m_commandBuffer = VK_NULL_HANDLE;
}
void CommandBuffer::Begin(VkCommandBufferUsageFlags usageFlag){
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usageFlag,
    };;
    vkBeginCommandBuffer(m_commandBuffer , &beginInfo);

};
void CommandBuffer::End(){
    vkEndCommandBuffer(m_commandBuffer);
}
void CommandBuffer::Reset(){
    vkResetCommandBuffer(m_commandBuffer , 0);
}