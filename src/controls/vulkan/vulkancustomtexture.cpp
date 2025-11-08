#include "vulkancustomtexture.h"

#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <fstream>



VulkanCustomTexture::VulkanCustomTexture(void* pvkinst, void* pphysDev,void*  pdev,int pframesInFlight)
{
    inst =  reinterpret_cast<QVulkanInstance *>(pvkinst);

    m_physDev = *static_cast<VkPhysicalDevice *>(pphysDev);
    m_dev = *static_cast<VkDevice *>(pdev);

    framesInFlight = pframesInFlight;



}

VulkanCustomTexture::~VulkanCustomTexture()
{
    m_devFuncs->vkDestroyBuffer(m_dev, m_vbuf, nullptr);
    m_devFuncs->vkDestroyBuffer(m_dev, m_ubuf, nullptr);
    m_devFuncs->vkFreeMemory(m_dev, m_vbufMem, nullptr);
    m_devFuncs->vkFreeMemory(m_dev, m_ubufMem, nullptr);

    m_devFuncs->vkDestroyPipelineCache(m_dev, m_pipelineCache, nullptr);
    m_devFuncs->vkDestroyPipelineLayout(m_dev, m_pipelineLayout, nullptr);
    m_devFuncs->vkDestroyPipeline(m_dev, m_pipeline, nullptr);

    m_devFuncs->vkDestroyRenderPass(m_dev, m_renderPass, nullptr);

    m_devFuncs->vkDestroyDescriptorSetLayout(m_dev, m_resLayout, nullptr);
    m_devFuncs->vkDestroyDescriptorPool(m_dev, m_descriptorPool, nullptr);

}



void VulkanCustomTexture::prepareShader(Stage stage)
{
    const char* filename = (stage == VertexStage)
    ? "squircle.vert.spv"
    : "squircle.frag.spv";

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file)
        throw std::runtime_error(std::string("Failed to read shader ") + filename);

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        throw std::runtime_error("Failed to read file content");

    if (stage == VertexStage)
        m_vert = std::move(buffer);
    else
        m_frag = std::move(buffer);
}


static const float vertices[] = {
    -1, -1,
    1, -1,
    -1, 1,
    1, 1
};

const int UBUF_SIZE = 4;


bool VulkanCustomTexture::buildTexture(int width, int height)
{
    VkImageCreateInfo imageInfo;
    memset(&imageInfo, 0, sizeof(imageInfo));
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.flags = 0;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.extent.width = uint32_t(width);
    imageInfo.extent.height = uint32_t(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImage image = VK_NULL_HANDLE;
    if (m_devFuncs->vkCreateImage(m_dev, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        qCritical("VulkanWrapper: failed to create image!");
        return  false;
    }

    m_texture = image;

    VkMemoryRequirements memReq;
    m_devFuncs->vkGetImageMemoryRequirements(m_dev, image, &memReq);

    quint32 memIndex = 0;
    VkPhysicalDeviceMemoryProperties physDevMemProps;
    inst->functions()->vkGetPhysicalDeviceMemoryProperties(m_physDev, &physDevMemProps);
    for (uint32_t i = 0; i < physDevMemProps.memoryTypeCount; ++i) {
        if (!(memReq.memoryTypeBits & (1 << i)))
            continue;
        memIndex = i;
    }

    VkMemoryAllocateInfo allocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        memReq.size,
        memIndex
    };

    VkResult err = m_devFuncs->vkAllocateMemory(m_dev, &allocInfo, nullptr, &m_textureMemory);
    if (err != VK_SUCCESS) {
        qWarning("Failed to allocate memory for linear image: %d", err);
        return false;
    }

    err = m_devFuncs->vkBindImageMemory(m_dev, image, m_textureMemory, 0);
    if (err != VK_SUCCESS) {
        qWarning("Failed to bind linear image memory: %d", err);
        return false;
    }

    VkImageViewCreateInfo viewInfo;
    memset(&viewInfo, 0, sizeof(viewInfo));
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = imageInfo.format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    err = m_devFuncs->vkCreateImageView(m_dev, &viewInfo, nullptr, &m_textureView);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create render target image view: %d", err);
        return false;
    }

    VkFramebufferCreateInfo fbInfo;
    memset(&fbInfo, 0, sizeof(fbInfo));
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = m_renderPass;
    fbInfo.attachmentCount = 1;
    fbInfo.pAttachments = &m_textureView;
    fbInfo.width = uint32_t(width);
    fbInfo.height = uint32_t(height);
    fbInfo.layers = 1;

    err = m_devFuncs->vkCreateFramebuffer(m_dev, &fbInfo, nullptr, &m_textureFramebuffer);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create framebuffer: %d", err);
        return false;
    }
    return true;
}

void VulkanCustomTexture::freeTexture()
{
    if (m_texture) {
        m_devFuncs->vkDestroyFramebuffer(m_dev, m_textureFramebuffer, nullptr);
        m_textureFramebuffer = VK_NULL_HANDLE;
        m_devFuncs->vkFreeMemory(m_dev, m_textureMemory, nullptr);
        m_textureMemory = VK_NULL_HANDLE;
        m_devFuncs->vkDestroyImageView(m_dev, m_textureView, nullptr);
        m_textureView = VK_NULL_HANDLE;
        m_devFuncs->vkDestroyImage(m_dev, m_texture, nullptr);
        m_texture = VK_NULL_HANDLE;
    }

}

bool VulkanCustomTexture::createRenderPass()
{
    const VkFormat vkformat = VK_FORMAT_R8G8B8A8_UNORM;
    const VkSampleCountFlagBits samples =  VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentDescription colorAttDesc;
    memset(&colorAttDesc, 0, sizeof(colorAttDesc));
    colorAttDesc.format = vkformat;
    colorAttDesc.samples = samples;
    colorAttDesc.loadOp =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttDesc.storeOp =  VK_ATTACHMENT_STORE_OP_STORE;
    colorAttDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    const VkAttachmentReference colorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpassDesc;
    memset(&subpassDesc, 0, sizeof(subpassDesc));
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &colorRef;
    subpassDesc.pDepthStencilAttachment = nullptr;
    subpassDesc.pResolveAttachments = nullptr;

    VkRenderPassCreateInfo rpInfo;
    memset(&rpInfo, 0, sizeof(rpInfo));
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.attachmentCount = 1;
    rpInfo.pAttachments = &colorAttDesc;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpassDesc;

    VkResult err = m_devFuncs->vkCreateRenderPass(m_dev, &rpInfo, nullptr, &m_renderPass);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create renderpass: %d", err);
        return false;
    }

    return true;
}



static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}


bool VulkanCustomTexture::initialize()
{
    m_initialized = true;

    //[1] 创建vulkan instance 已由qt窗口创建，直接获取
    Q_ASSERT(inst && inst->isValid());
    const VkInstance& vkinst = inst->vkInstance();

    //[2] 选择物理设备(即gpu)、逻辑设备 , 已由qt窗口创建， 直接获取
    Q_ASSERT(m_physDev && m_dev);

    //[3] 设备对应的函数接口
    m_devFuncs = inst->deviceFunctions(m_dev);
    m_funcs = inst->functions();
    Q_ASSERT(m_devFuncs && m_funcs);

    //[4] 创建渲染通道
    createRenderPass();

    //[5] 物理设备属性
    VkPhysicalDeviceProperties physDevProps;
    m_funcs->vkGetPhysicalDeviceProperties(m_physDev, &physDevProps);

    //[5] 逻辑设备属性
    VkPhysicalDeviceMemoryProperties physDevMemProps;
    m_funcs->vkGetPhysicalDeviceMemoryProperties(m_physDev, &physDevMemProps);

    //[6] 创建顶点buffer
    VkBufferCreateInfo bufferInfo;
    memset(&bufferInfo, 0, sizeof(bufferInfo));
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VkResult err = m_devFuncs->vkCreateBuffer(m_dev, &bufferInfo, nullptr, &m_vbuf);
    if (err != VK_SUCCESS)
        qFatal("Failed to create vertex buffer: %d", err);

    //[7] 顶点buffer内存请求信息
    VkMemoryRequirements memReq;
    m_devFuncs->vkGetBufferMemoryRequirements(m_dev, m_vbuf, &memReq);
    VkMemoryAllocateInfo allocInfo;
    memset(&allocInfo, 0, sizeof(allocInfo));
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;

    //[8] 顶点buffer选择内存类型 (连贯类型
    uint32_t memTypeIndex = uint32_t(-1);
    const VkMemoryType *memType = physDevMemProps.memoryTypes;
    for (uint32_t i = 0; i < physDevMemProps.memoryTypeCount; ++i) {
        if (memReq.memoryTypeBits & (1 << i)) {
            if ((memType[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                    && (memType[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                    memTypeIndex = i;
                    break;
            }
        }
    }
    if (memTypeIndex == uint32_t(-1))
        qFatal("Failed to find host visible and coherent memory type");

    //[9] 顶点buffer申请内存
    allocInfo.memoryTypeIndex = memTypeIndex;
    err = m_devFuncs->vkAllocateMemory(m_dev, &allocInfo, nullptr, &m_vbufMem);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate vertex buffer memory of size %u: %d", uint(allocInfo.allocationSize), err);

    //[9] 映射gpu中内存到p，把顶点数据复制进去，取消映射 ，绑定内存和buffer
    void *p = nullptr;
    err = m_devFuncs->vkMapMemory(m_dev, m_vbufMem, 0, allocInfo.allocationSize, 0, &p);
    if (err != VK_SUCCESS || !p)
        qFatal("Failed to map vertex buffer memory: %d", err);
    memcpy(p, vertices, sizeof(vertices));
    m_devFuncs->vkUnmapMemory(m_dev, m_vbufMem);
    err = m_devFuncs->vkBindBufferMemory(m_dev, m_vbuf, m_vbufMem, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind vertex buffer memory: %d", err);

    //[10]  创建片段着色器uniform buff, size = 设备内存对齐 * 飞帧（cpu处理还没渲染的帧数）
    //      和顶点buffer类似的步骤
    m_allocPerUbuf = aligned(UBUF_SIZE, physDevProps.limits.minUniformBufferOffsetAlignment);

    bufferInfo.size = framesInFlight * m_allocPerUbuf;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    err = m_devFuncs->vkCreateBuffer(m_dev, &bufferInfo, nullptr, &m_ubuf);
    if (err != VK_SUCCESS)
        qFatal("Failed to create uniform buffer: %d", err);
    m_devFuncs->vkGetBufferMemoryRequirements(m_dev, m_ubuf, &memReq);
    memTypeIndex = -1;
    for (uint32_t i = 0; i < physDevMemProps.memoryTypeCount; ++i) {
        if (memReq.memoryTypeBits & (1 << i)) {
            if ((memType[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                    && (memType[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                    memTypeIndex = i;
                    break;
            }
        }
    }
    if (memTypeIndex == uint32_t(-1))
        qFatal("Failed to find host visible and coherent memory type");

    allocInfo.allocationSize = qMax(memReq.size, framesInFlight * m_allocPerUbuf);
    allocInfo.memoryTypeIndex = memTypeIndex;
    err = m_devFuncs->vkAllocateMemory(m_dev, &allocInfo, nullptr, &m_ubufMem);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate uniform buffer memory of size %u: %d", uint(allocInfo.allocationSize), err);

    err = m_devFuncs->vkBindBufferMemory(m_dev, m_ubuf, m_ubufMem, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind uniform buffer memory: %d", err);

    // Now onto the pipeline.

    // [11] 创建管线缓存
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    memset(&pipelineCacheInfo, 0, sizeof(pipelineCacheInfo));
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    err = m_devFuncs->vkCreatePipelineCache(m_dev, &pipelineCacheInfo, nullptr, &m_pipelineCache);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline cache: %d", err);

    //[12] 创建布局描述
    VkDescriptorSetLayoutBinding descLayoutBinding;
    memset(&descLayoutBinding, 0, sizeof(descLayoutBinding));
    descLayoutBinding.binding = 0;
    descLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descLayoutBinding.descriptorCount = 1;
    descLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo layoutInfo;
    memset(&layoutInfo, 0, sizeof(layoutInfo));
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &descLayoutBinding;
    err = m_devFuncs->vkCreateDescriptorSetLayout(m_dev, &layoutInfo, nullptr, &m_resLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor set layout: %d", err);

    //[13] 创建管线布局
    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    memset(&pipelineLayoutInfo, 0, sizeof(pipelineLayoutInfo));
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_resLayout;
    err = m_devFuncs->vkCreatePipelineLayout(m_dev, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    if (err != VK_SUCCESS)
        qWarning("Failed to create pipeline layout: %d", err);

    VkGraphicsPipelineCreateInfo pipelineInfo;
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    VkShaderModuleCreateInfo shaderInfo;
    memset(&shaderInfo, 0, sizeof(shaderInfo));
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = m_vert.size();
    shaderInfo.pCode = reinterpret_cast<const quint32 *>(m_vert.data());
    VkShaderModule vertShaderModule;
    err = m_devFuncs->vkCreateShaderModule(m_dev, &shaderInfo, nullptr, &vertShaderModule);
    if (err != VK_SUCCESS)
        qFatal("Failed to create vertex shader module: %d", err);

    shaderInfo.codeSize = m_frag.size();
    shaderInfo.pCode = reinterpret_cast<const quint32 *>(m_frag.data());
    VkShaderModule fragShaderModule;
    err = m_devFuncs->vkCreateShaderModule(m_dev, &shaderInfo, nullptr, &fragShaderModule);
    if (err != VK_SUCCESS)
        qFatal("Failed to create fragment shader module: %d", err);

    VkPipelineShaderStageCreateInfo stageInfo[2];
    memset(&stageInfo, 0, sizeof(stageInfo));
    stageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stageInfo[0].module = vertShaderModule;
    stageInfo[0].pName = "main";
    stageInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stageInfo[1].module = fragShaderModule;
    stageInfo[1].pName = "main";
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stageInfo;

    VkVertexInputBindingDescription vertexBinding = {
        0, // binding
        2 * sizeof(float), // stride
        VK_VERTEX_INPUT_RATE_VERTEX
    };
    VkVertexInputAttributeDescription vertexAttr = {
        0, // location
        0, // binding
        VK_FORMAT_R32G32_SFLOAT, // 'vertices' only has 2 floats per vertex
        0 // offset
    };
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    memset(&vertexInputInfo, 0, sizeof(vertexInputInfo));
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBinding;
    vertexInputInfo.vertexAttributeDescriptionCount = 1;
    vertexInputInfo.pVertexAttributeDescriptions = &vertexAttr;
    pipelineInfo.pVertexInputState = &vertexInputInfo;

    VkDynamicState dynStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicInfo;
    memset(&dynamicInfo, 0, sizeof(dynamicInfo));
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.dynamicStateCount = 2;
    dynamicInfo.pDynamicStates = dynStates;
    pipelineInfo.pDynamicState = &dynamicInfo;

    VkPipelineViewportStateCreateInfo viewportInfo;
    memset(&viewportInfo, 0, sizeof(viewportInfo));
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = viewportInfo.scissorCount = 1;
    pipelineInfo.pViewportState = &viewportInfo;

    VkPipelineInputAssemblyStateCreateInfo iaInfo;
    memset(&iaInfo, 0, sizeof(iaInfo));
    iaInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    iaInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    pipelineInfo.pInputAssemblyState = &iaInfo;

    VkPipelineRasterizationStateCreateInfo rsInfo;
    memset(&rsInfo, 0, sizeof(rsInfo));
    rsInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rsInfo.lineWidth = 1.0f;
    pipelineInfo.pRasterizationState = &rsInfo;

    VkPipelineMultisampleStateCreateInfo msInfo;
    memset(&msInfo, 0, sizeof(msInfo));
    msInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipelineInfo.pMultisampleState = &msInfo;

    VkPipelineDepthStencilStateCreateInfo dsInfo;
    memset(&dsInfo, 0, sizeof(dsInfo));
    dsInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pipelineInfo.pDepthStencilState = &dsInfo;

    // SrcAlpha, One
    VkPipelineColorBlendStateCreateInfo blendInfo;
    memset(&blendInfo, 0, sizeof(blendInfo));
    blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    VkPipelineColorBlendAttachmentState blend;
    memset(&blend, 0, sizeof(blend));
    blend.blendEnable = true;
    blend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    blend.colorBlendOp = VK_BLEND_OP_ADD;
    blend.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blend.alphaBlendOp = VK_BLEND_OP_ADD;
    blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
            | VK_COLOR_COMPONENT_A_BIT;
    blendInfo.attachmentCount = 1;
    blendInfo.pAttachments = &blend;
    pipelineInfo.pColorBlendState = &blendInfo;

    pipelineInfo.layout = m_pipelineLayout;

    pipelineInfo.renderPass = m_renderPass;

    err = m_devFuncs->vkCreateGraphicsPipelines(m_dev, m_pipelineCache, 1, &pipelineInfo, nullptr, &m_pipeline);

    m_devFuncs->vkDestroyShaderModule(m_dev, vertShaderModule, nullptr);
    m_devFuncs->vkDestroyShaderModule(m_dev, fragShaderModule, nullptr);

    if (err != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", err);

    // Now just need some descriptors.
    VkDescriptorPoolSize descPoolSizes[] = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1 }
    };
    VkDescriptorPoolCreateInfo descPoolInfo;
    memset(&descPoolInfo, 0, sizeof(descPoolInfo));
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.flags = 0; // won't use vkFreeDescriptorSets
    descPoolInfo.maxSets = 1;
    descPoolInfo.poolSizeCount = sizeof(descPoolSizes) / sizeof(descPoolSizes[0]);
    descPoolInfo.pPoolSizes = descPoolSizes;
    err = m_devFuncs->vkCreateDescriptorPool(m_dev, &descPoolInfo, nullptr, &m_descriptorPool);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor pool: %d", err);

    VkDescriptorSetAllocateInfo descAllocInfo;
    memset(&descAllocInfo, 0, sizeof(descAllocInfo));
    descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descAllocInfo.descriptorPool = m_descriptorPool;
    descAllocInfo.descriptorSetCount = 1;
    descAllocInfo.pSetLayouts = &m_resLayout;
    err = m_devFuncs->vkAllocateDescriptorSets(m_dev, &descAllocInfo, &m_ubufDescriptor);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate descriptor set");

    VkWriteDescriptorSet writeInfo;
    memset(&writeInfo, 0, sizeof(writeInfo));
    writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo.dstSet = m_ubufDescriptor;
    writeInfo.dstBinding = 0;
    writeInfo.descriptorCount = 1;
    writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    VkDescriptorBufferInfo bufInfo;
    bufInfo.buffer = m_ubuf;
    bufInfo.offset = 0; // dynamic offset is used so this is ignored
    bufInfo.range = UBUF_SIZE;
    writeInfo.pBufferInfo = &bufInfo;

    m_devFuncs->vkUpdateDescriptorSets(m_dev, 1, &writeInfo, 0, nullptr);
    return true;
}

void VulkanCustomTexture::sync()
{
    if (!m_initialized) {
        prepareShader(VertexStage);
        prepareShader(FragmentStage);
        initialize();
        m_initialized = true;
    }
}

void VulkanCustomTexture::render(VkCommandBuffer* p_cmdBuf,uint32_t currentFrameSlot,int width,int height)
{
    VkCommandBuffer cmdBuf = * p_cmdBuf;
    if (!m_initialized)
        return;

    VkResult err = VK_SUCCESS;

    VkDeviceSize ubufOffset = currentFrameSlot * m_allocPerUbuf;
    void *p = nullptr;
    err = m_devFuncs->vkMapMemory(m_dev, m_ubufMem, ubufOffset, m_allocPerUbuf, 0, &p);
    if (err != VK_SUCCESS || !p)
        qFatal("Failed to map uniform buffer memory: %d", err);
    float t = m_t;
    memcpy(p, &t, 4);
    m_devFuncs->vkUnmapMemory(m_dev, m_ubufMem);

    VkClearValue clearColor = {{ {0, 0, 0, 1} }};

    VkRenderPassBeginInfo rpBeginInfo = {};
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = m_renderPass;
    rpBeginInfo.framebuffer = m_textureFramebuffer;
    rpBeginInfo.renderArea.extent.width = width;
    rpBeginInfo.renderArea.extent.height = height;
    rpBeginInfo.clearValueCount = 1;
    rpBeginInfo.pClearValues = &clearColor;


    m_devFuncs->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    m_devFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    VkDeviceSize vbufOffset = 0;
    m_devFuncs->vkCmdBindVertexBuffers(cmdBuf, 0, 1, &m_vbuf, &vbufOffset);

    uint32_t dynamicOffset = m_allocPerUbuf * currentFrameSlot;
    m_devFuncs->vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
                                        &m_ubufDescriptor, 1, &dynamicOffset);

    VkViewport vp = { 0, 0, float(width), float(height), 0.0f, 1.0f };
    m_devFuncs->vkCmdSetViewport(cmdBuf, 0, 1, &vp);
    VkRect2D scissor = { { 0, 0 }, { uint32_t(width), uint32_t(height) } };
    m_devFuncs->vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

    m_devFuncs->vkCmdDraw(cmdBuf, 4, 1, 0, 0);
    m_devFuncs->vkCmdEndRenderPass(cmdBuf);

    // Memory barrier before the texture can be used as a source.
    // Since we are not using a sub-pass, we have to do this explicitly.

    VkImageMemoryBarrier imageTransitionBarrier = {};
    imageTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageTransitionBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageTransitionBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    imageTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageTransitionBarrier.image = m_texture;
    imageTransitionBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageTransitionBarrier.subresourceRange.levelCount = imageTransitionBarrier.subresourceRange.layerCount = 1;

    m_devFuncs->vkCmdPipelineBarrier(cmdBuf,
                                     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                     0, 0, nullptr, 0, nullptr,
                                     1, &imageTransitionBarrier);
}



VkImage VulkanCustomTexture::texture() const
{
    return m_texture;
}

float VulkanCustomTexture::t() const
{
    return m_t;
}

void VulkanCustomTexture::setT(float newT)
{
    m_t = newT;
}
