#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <cstring>
#include <exception>

#include "./command_buffer.h"

class Swapchain;
class CommandBuffer;
class ISurfaceProvider;

class VulkanContext{
    public:
        const uint32_t MaxInflightFrames = 2;
        static VulkanContext& Get();
        //initilize
        void Initialize(const char* appName , ISurfaceProvider* surfacePrivder);
        void Cleanup();
        void RecreateSwapchain();

        //vulkanobject
        VkInstance GetVkInstance() const { return m_vkInstance;}
    //定義
        struct FrameContext{
            std::shared_ptr<CommandBuffer> commandBuffer;
            VkFence inflightFence = VK_NULL_HANDLE;
        };
    private:

    private:
        void CreateInstance(const char* appName);
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateDebugMessenger();
        void CreateCommandPool();
        void CreateDescriptorPool();
        void CreateFrameContexts();
        void DestoryFrameContexts();

        void AdvanceFrame();
        void BuildVkFeatures();

        ISurfaceProvider* m_surfaceProvider{};
        VkInstance m_vkInstance{};
        VkPhysicalDevice m_vkPhysicalDevice{};
        VkDevice m_vkDevice{};
        VkQueue m_graphicsQueue{};
        uint32_t m_graphicsQueueFamilyIndex{};
        uint32_t m_presentQueueFamilyIndex{};

        VkPhysicalDeviceMemoryProperties m_memoryProperties{};
        VkPhysicalDeviceProperties m_physicalDeviceProperties{};

        VkSurfaceKHR m_surface{};
        VkCommandPool m_commandPool{};
        VkDescriptorPool m_descriptorPool{};
        std::vector<FrameContext> m_frameContext;
        std::unique_ptr<Swapchain> m_swapchain;

    };