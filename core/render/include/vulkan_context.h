#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "./command_buffer.h"
#include "./surface_provider.h"

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
        VkDevice GetVkDevice() const {return m_vkDevice;}
        VkPhysicalDevice GetVkPhysicalDevice() const {return m_vkPhysicalDevice;}
        VkDescriptorPool GetVkDescriptorPool() const {return m_descriptorPool;}

        VkQueue GetVkGraphicsQueue() const {return m_graphicsQueue;}
        uint32_t GetGraphicsFamily() const {return m_graphicsQueueFamilyIndex;}
        uint32_t GetPresentFamily() const {return m_presentQueueFamilyIndex;}

        VkCommandPool GetCommandPool() const {return m_commandPool;}
        VkSurfaceKHR GetSurface() const {return m_surface;}
        //commnad buffer create
        std::shared_ptr<CommandBuffer> CreateCommandBuffer();
        void FreeDescriptorSet(VkDescriptorSet descriptorSet);

    //定義
        struct FrameContext{
            std::shared_ptr<CommandBuffer> commandBuffer;
            VkFence inflightFence = VK_NULL_HANDLE;
        };
        uint32_t GetCurrentFrameIndex() const {return m_currentFrameIndex;}
        VkResult AcquireNextImage();
        
        //command 実行
        void SubmitPresent();
        void SubmitAndWait();
        FrameContext* GetCurrentFrameContext();
        //swapchain
        std::unique_ptr<Swapchain>& GetSwapchain() {return m_swapchain;}
        //memory type
        uint32_t FindMemoryType(const VkMemoryRequirements& requirements , VkMemoryPropertyFlags properties) const;

        //function call back
        std::function<void(std::vector<const char*>&)> GetWindowSystemExtensions;
        void SetDebugObjectName(void* objectHandle, VkObjectType type , const char* name);
    private:
        VulkanContext() = default;
        ~VulkanContext() = default;

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
        VkDebugUtilsMessengerEXT m_debugMessenger{};
        PFN_vkSetDebugUtilsObjectNameEXT m_pfnSetDebugUtilsObjectNameEXT{};

        uint32_t m_currentFrameIndex = 0;
        // --debug
        VkPhysicalDeviceFeatures2 m_physDevFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2
        };
        VkPhysicalDeviceVulkan11Features m_vulkan11Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES
        };
        VkPhysicalDeviceVulkan12Features m_vulkan12Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
        };
        VkPhysicalDeviceVulkan13Features m_vulkan13Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
        };
        VkPhysicalDeviceShaderAtomicFloatFeaturesEXT m_atomicFloatFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT
        };
    };