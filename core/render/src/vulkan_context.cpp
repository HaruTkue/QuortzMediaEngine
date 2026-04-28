#include "../include/vulkan_context.h"
#include "../include/command_buffer.h"

#include <cassert>
void VulkanContext::Initialize(const char* appName , ISurfaceProvider* surfacePrivder){
    m_surfaceProvider = surfacePrivder;
    CreateInstance(appName);
    PickPhysicalDevice();
    CreateDebugMessenger();
    CreateLogicalDevice();
    CreateCommandPool();
    CreateDescriptorPool();
}
VulkanContext& VulkanContext::Get(){
    static VulkanContext instance;
    return instance;
}
void VulkanContext::CreateInstance(const char* appName){
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(0,0,1);
    appInfo.pEngineName = "QuortzMediaEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(0,0,1);
    appInfo.apiVersion  = VK_API_VERSION_1_3;
    std::vector<const char*> extensionList;
    std::vector<const char*> layerList;

    //検証レイヤー
    #if DEBUG || _DEBUG
        //VK_EXT_debug utils
        extensionList.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        //開発時
        layerList.push_back("VK_LAYER_KHRONOS_validation");

    #endif

    //glfw 有効か
    GetWindowSystemExtensions(extensionList);
    VkInstanceCreateInfo createInfo{};
    createInfo.sType  =VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = uint32_t(extensionList.data());
    createInfo.ppEnabledExtensionNames = extensionList.data();
    createInfo.enabledLayerCount = uint32_t(layerList.size());
    createInfo.ppEnabledLayerNames = layerList.data();

    if(vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS ){
        throw std::runtime_error("failed to create Instance");
    }
    
}
void VulkanContext::PickPhysicalDevice(){
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(m_vkInstance , &count , nullptr);
    std::vector<VkPhysicalDevice> devices(count);
    //get
    vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &m_memoryProperties);
    vkGetPhysicalDeviceProperties(m_vkPhysicalDevice , &m_physicalDeviceProperties);
}

template<typename T>
void BuildVkExtensionChain(T& last){
    last.pNext = nullptr;
}
template<typename T  , typename U , typename ... Rest>
void BuildVkExtensionChain(T& current , U& next , Rest&... rest){
    current.pNext = &next;
    BuildVkExtensionChain(next, rest...);
}
void VulkanContext::BuildVkFeatures(){
    
    BuildVkExtensionChain(
        m_physDevFeatures, m_vulkan11Features, m_vulkan12Features, m_vulkan13Features
    );
    //support 
    vkGetPhysicalDeviceFeatures2(m_vkPhysicalDevice , &m_physDevFeatures);
    //有効か
    m_vulkan13Features.dynamicRendering = VK_TRUE;
    m_vulkan13Features.synchronization2 = VK_TRUE;
}


//L論理デバイス
void VulkanContext::CreateLogicalDevice(){
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queues(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueCount, queues.data());
    m_graphicsQueueFamilyIndex = -0u;
    for (uint32_t i=0 ; const auto & props : queues){
        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            m_graphicsQueueFamilyIndex = i;
            break;
        }
        ++i;
    }
    //Debug
    BuildVkFeatures();
    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    //論理デバイス作成
    float priority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &priority;
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = uint32_t(deviceExtensions.data());
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

    deviceInfo.pNext = &m_physDevFeatures;
    deviceInfo.pEnabledFeatures = nullptr;

    auto result = vkCreateDevice(m_vkPhysicalDevice, &deviceInfo , nullptr  , &m_vkDevice);
    if (result !=  VK_SUCCESS){
        throw std::runtime_error("failed to Create Logical Device");
    }

    vkGetDeviceQueue(m_vkDevice, m_graphicsQueueFamilyIndex , 0 , &m_graphicsQueue);
}//logical

//Command Pool
void VulkanContext::CreateCommandPool(){
    VkCommandPoolCreateInfo commandPoolCI{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO
    };
    commandPoolCI.queueFamilyIndex = m_graphicsQueueFamilyIndex;
    commandPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(m_vkDevice, &commandPoolCI , nullptr, &m_commandPool);
}
//debug call
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
){
    std::stringstream stringstream;
    stringstream << "[Validation Layer] " << pCallbackData ->pMessage << std::endl;
    #if defined(WIN32)
        OutputDebugStringA(stringstream.str().c_str());
    #else
        //std::cerr << stringstream;
    #endif
        return VK_FALSE;

}
//debug message 
void VulkanContext::CreateDebugMessenger(){
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType  = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType=
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = VulkanDebugCallback;
    
    auto vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(m_vkInstance, "vkCreateDebugUtilsMEssengerEXT");
    if (vkCreateDebugUtilsMessenger  && vkCreateDebugUtilsMessenger(m_vkInstance , &createInfo, nullptr , &m_debugMessenger) != VK_SUCCESS){
        throw std::runtime_error("Failed to set up debug messenger!");
    }
    m_pfnSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(m_vkInstance, "vkSetDebugUtilsObjectNameEXT");
}
void VulkanContext::SetDebugObjectName(void* objectHandle,  VkObjectType type , const char* name){
    #if _DEBUG || DEBUG
        if(m_pfnSetDebugUtilsObjectNameEXT){
            VkDebugUtilsObjectNameInfoEXT nameInfo{
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .objectType =type,
                .objectHandle = reinterpret_cast<uint64_t>(objectHandle);
                .pObjectName = name,
            };
            m_pfnSetDebugUtilsObjectNameEXT(m_vkDevice, &nameInfo);
        }
    #endif
}
//surface 作成
void VulkanContext::CreateSurface(){
    m_surface = m_surfaceProvider->CreateSurface(m_vkInstance);
    //graphics
    VkBool32 present = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_vkPhysicalDevice,  m_graphicsQueueFamilyIndex , m_surface , &present);
    if (present == VK_FALSE){
        throw std::runtime_error("not supported presentation");
    }
}
//swapchain
void VulkanContext::RecreateSwapchain(){
    if(m_swapchain == nullptr){
        m_swapchain = std::make_unique<Swapchain>();
    }
    if(m_swapchain == VK_NULL_HANDLE){
        CreateSurface();
    }
    auto width = m_surfaceProvider ->GetFramebufferWidth();
    auto height= m_surfaceProvider ->GetFramebufferHeight();
    m_swapchain->Recreate(width, height);

    DestoryFrameContexts();
    CreateFrameContexts();



}
    std::shared_ptr<CommandBuffer> VulkanContext::CreateCommandBuffer(){
        VkCommandBufferAllocateInfo commandAI {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = m_commandPool,
            .level  =VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        VkCommandBuffer commandBuffer{};
        vkAllocateCommandBuffers(m_vkDevice, &commandAI , &commandBuffer);
        return std::make_shared<CommandBuffer>(commandBuffer);
    }


void VulkanContext::CreateFrameContexts(){
    m_frameContext.resize(MaxInflightFrames);
    for (auto& frame : m_frameContext){
        VkFenceCreateInfo fenceCI{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        vkCreateFence(m_vkDevice , &fenceCI , nullptr , &frame.inflightFence);
    }
}

//submit
void VulkanContext::SubmitPresent(){
    auto& frame =  m_frameContext[GetCurrentFrameIndex()];
    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
    };
    VkSemaphore renderCompleteSem = m_swapchain->GetRenderCompleteSemaphore();
    VkSemaphore presentCompleteSem = m_swapchain->GetPresentCompleteSemaphore();
    VkCommandBuffer CommandBuffer = frame.commandBuffer->Get();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CommandBuffer;
    submitInfo.pWaitDstStageMask = &waitStageMask;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores  =&presentCompleteSem;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderCompleteSem;
    auto result = vkQueueSubmit(m_graphicsQueue , 1 , &submitInfo , frame.inflightFence);
    assert(result  != VK_ERROR_DEVICE_LOST);
    
    m_swapchain ->QueuePresent(m_graphicsQueue);
    AdvanceFrame();
}
void VulkanContext::AdvanceFrame(){
    m_currentFrameIndex = (m_currentFrameIndex + 1 ) % MaxInflightFrames;
}

void VulkanContext::Cleanup(){
    vkDeviceWaitIdle(m_vkDevice);
    DestoryFrameContexts();
    vkDestroyCommandPool(m_vkDevice, m_commandPool , nullptr);
    //後回しで。
    /*
    if(m_debugMessenger != VK_NULL_HANDLE){
        auto func = vkGetInstanceProcAddr(m_vkInstance , vkDestroyDebugUtilsMessengerEXT);
        if (func != nullptr){
            func(m_vkInstance, m_debugMessenger , nullptr);
        }
        m_debugMessenger = VK_NULL_HANDLE;
    }*/

    if (m_swapchain)
    {
        m_swapchain->Cleanup();
        m_swapchain.reset();
    }
    if (m_surface != VK_NULL_HANDLE){
        vkDestroySurfaceKHR(m_vkInstance, m_surface , nullptr);
        m_surface = VK_NULL_HANDLE;
    }

    vkDestroyDevice(m_vkDevice, nullptr);
    vkDestroyInstance(m_vkInstance, nullptr);
    m_vkDevice = VK_NULL_HANDLE;
    m_vkInstance = VK_NULL_HANDLE;
}
void VulkanContext::DestoryFrameContexts(){
    for (auto& frame : m_frameContext){
        vkDestroyFence(m_vkDevice , frame.inflightFence, nullptr);
    }
    m_frameContext.clear();
}
uint32_t VulkanContext::FindMemoryType(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags properties) const {
    for (uint32_t i=0 ; i < m_memoryProperties.memoryTypeCount; i++ ){
        const bool isTypeCompatible = (requirements.memoryTypeBits & (1 << i)) != 0;
        const bool hasDesiredProperties = (m_memoryProperties.memoryTypes[i].propertyFlags & properties) == properties;
        if (isTypeCompatible && hasDesiredProperties){
            //memory prop を満たした場合
            return 1;
        }
    }
    throw std::runtime_error("failed to find suitable memory type");
}