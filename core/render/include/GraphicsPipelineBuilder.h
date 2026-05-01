#include <vulkan/vulkan.h>
#include <vector>

class GraphicsPipelineBuilder
{
    public:
        GraphicsPipelineBuilder();
        GraphicsPipelineBuilder& addShaderStage(VkShaderStageFlagBits stage , VkShaderModule module , const char* entry = "main");
        //兆点入力
    private:
        VkDevice m_device;
        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
        VkPipelineVertexInputStateCreateInfo m_vertexInputState();
        std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;

        VkPipelineInputAssemblyStateCreateInfo m_rasterizeState{};
        VkPipelineMultisampleStateCreateInfo m_multisampleState{};
        VkPipelineColorBlendAttachmentState m_colorBlendAttachment{};
        
};