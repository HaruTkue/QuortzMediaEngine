#include <vulkan/vulkan.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include "../../render/include/vulkan_context.h"

namespace loader{
    VkShaderModule LoadShaderModule(const std::filesystem::path& shaderSpvPath){
        //file load
        std::ifstream file(shaderSpvPath , std::ios::ate | std::ios::binary);
        if (!file.is_open()){
            throw std::runtime_error("Failed to open shader file:" +shaderSpvPath.string());
        }
        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(0).read(buffer.data() , fileSize);
        file.close();

        //vkshader module create info
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = buffer.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
        VkDevice device = VulkanContext::Get().GetVkDevice();
        VkShaderModule shaderModule{};
        auto result = vkCreateShaderModule(device , &createInfo , nullptr , &shaderModule);
        if(result != VK_SUCCESS){
            throw std::runtime_error("failed tto create shader module from:" + shaderSpvPath.string());
        }
        return shaderModule;
    }
}