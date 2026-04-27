#include "../include/swapchain.h"
#include <cassert>
bool Swapchain::Recreate(uint32_t width, uint32_t height){
    auto& vulkanCtx = VulkanContext::Get();
    auto VkPhysicalDevice = vulkanCtx.GetVkPhysicalDevice();
    auto vkDevice = vulkanCtx.GetVkDevice();
    auto surface = vulkanCtx.GetSurface();

    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice, surface ,&caps);
    VkExtent2D extent = caps.currentExtent;
    if (extent.width == UINT32_MAX){
        extent.width = width;
        extent.height= height;
    }
    uint32_t count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice, surface , &count , nullptr);
    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice, surface , &count , formats.data());


    //出力
    VkSurfaceFormatKHR format = formats[0];
    for (auto& surfaceFormat : formats){
        if(surfaceFormat.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            continue;
        }
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM || surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM){
            format = surfaceFormat;
            break;
        }
    }
    auto imageCount = std::max(3u , caps.minImageCount);
    VkSwapchainCreateInfoKHR info {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface;
    info.minImageCount = caps.minImageCount + 1;
    info.imageFormat = format.format;
    info.imageExtent = extent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    info.clipped = VK_TRUE;
    info.oldSwapchain = m_swapchain;

    //gpu idle 
    vkDeviceWaitIdle(vkDevice);
    VkSwapchainKHR swapchain{};
    if (vkCreateSwapchainKHR(vkDevice, &info, nullptr,&swapchain) != VK_SUCCESS){
        throw std::runtime_error("Failed to create swapchain");
    }
    //image get8
    m_swapchain = swapchain;
    m_imageFormat = format;
    m_imageExtent = extent;

    vkGetSwapchainImagesKHR(vkDevice , m_swapchain , &imageCount , nullptr);
    for (uint32_t i =0 ; i< m_images.size() ; i++){
        VkImageViewCreateInfo imageViewCI{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format.format,
            .components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        VkImageView view ;
        vkCreateImageView(vkDevice,  &imageViewCI , nullptr , &view);
        m_imageViews.push_back(view);
    }
    CreateFrameContext();
    return true;
};//SW recreate

//frame初期化
void Swapchain::CreateFrameContext(){
    auto& vulkanCtx = VulkanContext::Get();
    auto vkDevice = vulkanCtx.GetVkDevice();
    m_frames.resize(m_images.size());
    for (auto& frame : m_frames){
        VkSemaphoreCreateInfo semCI{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        vkCreateSemaphore(vkDevice , &semCI, nullptr , &frame.renderComplete);
    }
    uint32_t presentCompleteSemaphoreCount = m_images.size() + 1;
    m_presentSemaphoreList.reserve(presentCompleteSemaphoreCount);
    for (uint32_t i=0 ; i < presentCompleteSemaphoreCount; i++){
        VkSemaphoreCreateInfo semaphoreCI{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        VkSemaphore semaphore;
        vkCreateSemaphore(vkDevice , &semaphoreCI , nullptr, &semaphore);
        m_presentSemaphoreList.push_back(semaphore);
    }
}
//hakia

void Swapchain::DestroyFrameContext(){
    auto& vulkanCtx = VulkanContext::Get();
    auto vkDevice = vulkanCtx.GetVkDevice();
    for (auto& frame : m_frames){
        vkDestroySemaphore(vkDevice, frame.presentComplete, nullptr);
        vkDestroySemaphore(vkDevice, frame.renderComplete, nullptr);
    }
    m_frames.clear();
    for (auto& sem  : m_presentSemaphoreList){
        vkDestroySemaphore(vkDevice , sem , nullptr);
    }
    m_presentSemaphoreList.clear();
}
VkResult Swapchain::AcquireNextImage(){
    auto& vulkanCtx = VulkanContext::Get();
    auto vkDevice = vulkanCtx.GetVkDevice();
    //present
    assert(!m_presentSemaphoreList.empty());
    VkSemaphore acquireSemaphore = m_presentSemaphoreList.back();
    m_presentSemaphoreList.pop_back();
    auto result = vkAcquireNextImageKHR(vkDevice , m_swapchain , UINT64_MAX , acquireSemaphore , VK_NULL_HANDLE , &m_currentIndex);
     if(result != VK_SUCCESS){
        m_presentSemaphoreList.push_back(acquireSemaphore);
        return result;
     }

     //置き換え
     VkSemaphore oldSemaphore = m_frames[m_currentIndex].presentComplete;
     if (oldSemaphore != VK_NULL_HANDLE){
        m_presentSemaphoreList.push_back(oldSemaphore);
     }
     m_frames[m_currentIndex].presentComplete = acquireSemaphore;
     return result;
}

VkResult VulkanContext::AcquireNextImage(){
    auto* frame = GetCurrentFrameContext();
    auto fence = frame->inflightFence;
    vkWaitForFences(m_vkDevice, 1 , &fence , VK_TRUE , UINT64_MAX);

    auto result = m_swapchain->AcquireNextImage();
    if ( result == VK_SUCCESS){
        vkResetFences(m_vkDevice, 1 , &fence);
    }
    else if(result == VK_ERROR_OUT_OF_DATE_KHR){
        //省略
    }
    assert(result != VK_ERROR_DEVICE_LOST);
    return result;
}

//queue present
VkResult Swapchain::QueuePresent(VkQueue queuePresent){
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain; 
    presentInfo.pImageIndices = &m_currentIndex;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_frames[m_currentIndex].renderComplete;

    auto&vulkanCtx = VulkanContext::Get();
    auto result = vkQueuePresentKHR(queuePresent , &presentInfo);
    return result;
}
VkSemaphore Swapchain::GetPresentCompleteSemaphore() const{
    return m_frames[m_currentIndex].presentComplete;
}
VkSemaphore Swapchain::GetRenderCompleteSemaphore() const {
    return m_frames[m_currentIndex].renderComplete;
}
void Swapchain::Cleanup(){
    auto& vulkanCtx = VulkanContext::Get();
    auto vkDevice = vulkanCtx.GetVkDevice();
    for (auto& view: m_imageViews){
        vkDestoryImageView(vkDevice, view , nullptr);
    }
    if (m_swapchain){
        vkDestorySwapchainKHR(vkDevice,  m_swapchain , nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
    m_images.clear();
    m_imageViews.clear();
}