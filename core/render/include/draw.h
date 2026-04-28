#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "./buffer_resource.h"
class Draw {
    public:
        virtual void OnInitilize();
        virtual void OnDrawFrame();
        virtual void OnCleanup();
    private:
        void InitializeVertexBuffer();
        void InitializeGraphicsPipeline();

        std::shared_ptr<VertexBuffer> m_vertexBuffer;
        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};