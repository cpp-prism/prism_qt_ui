#ifndef VULKANCUSTOMTEXTURE_H
#define VULKANCUSTOMTEXTURE_H

#include <vector>
#include <QVulkanInstance>
#include <QVulkanDeviceFunctions>
#include <QVulkanFunctions>

class VulkanCustomTexture
{
public:
    VulkanCustomTexture(void* pvkinst, void* pphysDev,void*  pdev,int framesInFlight);
    virtual ~VulkanCustomTexture();


    enum Stage {
        VertexStage,
        FragmentStage
    };
    void prepareShader(Stage stage);
    bool buildTexture(int width, int height);
    void freeTexture();
    bool createRenderPass();
    bool initialize();
    void sync();
    void render(VkCommandBuffer* p_cmdBuf,uint32_t currentFrameSlot,int width,int height);
    VkImage texture() const;

    float t() const;
    void setT(float newT);

private:
    QVulkanInstance* inst ;
    int framesInFlight = -1;

    std::vector<uint8_t> m_vert;
    std::vector<uint8_t> m_frag;

    VkImage m_texture = VK_NULL_HANDLE;
    VkDeviceMemory m_textureMemory = VK_NULL_HANDLE;
    VkFramebuffer m_textureFramebuffer = VK_NULL_HANDLE;
    VkImageView m_textureView = VK_NULL_HANDLE;

    bool m_initialized = false;

    float m_t;

    VkPhysicalDevice m_physDev = VK_NULL_HANDLE;
    VkDevice m_dev = VK_NULL_HANDLE;
    QVulkanDeviceFunctions *m_devFuncs = nullptr;
    QVulkanFunctions *m_funcs = nullptr;

    VkBuffer m_vbuf = VK_NULL_HANDLE;
    VkDeviceMemory m_vbufMem = VK_NULL_HANDLE;
    VkBuffer m_ubuf = VK_NULL_HANDLE;
    VkDeviceMemory m_ubufMem = VK_NULL_HANDLE;
    VkDeviceSize m_allocPerUbuf = 0;

    VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;

    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_resLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;

    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_ubufDescriptor = VK_NULL_HANDLE;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;
};

#endif // VULKANCUSTOMTEXTURE_H
