#include "../include/buffer_resource.h"
template<typename T>
void BufferResource<T>::Cleanup(){
    VulkanContext& vulkanCtx = vulkanCOntext::Get();
    VkDevice device = vulkanCtx.GetVkDevice();

    if(m_buffer != VK_NULL_HANDLE){
        vkDestroyBuffer(device,  m_buffer , nullptr);
        m_buffer = VK_NULL_HANDLE;
    }
    if(m_memory != VK_NULL_HANDLE){
        vkFreeMEmory(device , m_memory,  nullptr);
        m_memory = VK_NULL_HANDLE;
    }
    m_size = 0;
}
template<typename T>
VkDescriptorBufferInfo BufferResource<T>::GetDescriptorInfo() const{
    return VkDescriptorBufferInfo{
        .buffer = m_buffer,
        .offset = 0,
        .range = m_size
    };
}
template<typename T>
bool BufferResource<T>::CreateBuffer(const VkBufferCreateInfo& createInfo ,VkMemoryPropertyFlags memProps){
    VulkanContext& vulkanCtx = VulkanContext::Get();
    VkDevice device = vulkanCtx.GetVkDevice();
    auto result = vkCreateBuffer(device,  &createInfo, nullptr, &m_buffer);
    if(result != VK_SUCCESS){
        return false;
    }
    //memory get
    VkMEmoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device,  m_buffer, &memRequirements);
    VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = vulkanCtx.FindMemoryType (memRequirements , memProps),
    };

    result = vkAllocateMemory(device,  &allocInfo, nullptr,  &m_memory);
    if (result != VK_SUCCESS){
        return false;
    }
    vkBindBufferMemory(device , m_buffer, m_memory , 0);
    m_size = createInfo.size;
    m_memProps = memProps;

    return true;
}