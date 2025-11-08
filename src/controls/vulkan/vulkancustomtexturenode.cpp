//#include "vulkancustomtexture.h"
#include <rs_vk_engine/rsvulkancustomtexture.h>
#include "vulkancustomtexturenode.h"
#include "vulkanCustomTextureItem.h"
#include <QtGui/QScreen>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/QSGSimpleTextureNode>
#include <QFile>
#include <QQuickItem>

VulkanCustomTextureNode::VulkanCustomTextureNode(QQuickItem *item)
    : m_item(item)
{
    m_window = m_item->window();
    connect(m_window, &QQuickWindow::beforeRendering, this, &VulkanCustomTextureNode::render);
    connect(m_window, &QQuickWindow::screenChanged, this, [this]() {
        if (m_window->effectiveDevicePixelRatio() != m_dpr)
            m_item->update();
    });

    //[1] 创建vulkan instance 已由qt窗口创建，直接获取
    QSGRendererInterface *rif = m_window->rendererInterface();
    QVulkanInstance *inst = reinterpret_cast<QVulkanInstance *>(
                rif->getResource(m_window, QSGRendererInterface::VulkanInstanceResource));
    Q_ASSERT(inst && inst->isValid());


    const int framesInFlight = m_window->graphicsStateInfo().framesInFlight;


    m_vk_texture = new RSVulkanCustomTexture(inst->vkInstance(),
                                           rif->getResource(m_window, QSGRendererInterface::PhysicalDeviceResource),
                                           rif->getResource(m_window, QSGRendererInterface::DeviceResource),
                                           framesInFlight);
}

VulkanCustomTextureNode::~VulkanCustomTextureNode()
{
    delete texture();
    if(m_vk_texture)
    {
        m_vk_texture->freeTexture();
        delete m_vk_texture;
    }
}

QSGTexture *VulkanCustomTextureNode::texture() const
{
    return QSGSimpleTextureNode::texture();
}


void VulkanCustomTextureNode::sync()
{
    m_dpr = m_window->effectiveDevicePixelRatio();
    const QSize newSize = m_window->size() * m_dpr;
    bool needsNew = false;

    m_vk_texture->sync();

    if (!texture())
        needsNew = true;

    if (newSize.width() != m_width || newSize.height() != m_height) {
        needsNew = true;
        m_width = newSize.width();
        m_height = newSize.height();
    }

    if (needsNew) {
        delete texture();
        m_vk_texture->freeTexture();
        m_vk_texture->buildTexture(newSize.width(),newSize.height());
        QSGTexture *wrapper = QNativeInterface::QSGVulkanTexture::fromNative(m_vk_texture->texture(),
                                                                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                                               m_window,
                                                                               newSize);
        setTexture(wrapper);
        Q_ASSERT(wrapper->nativeInterface<QNativeInterface::QSGVulkanTexture>()->nativeImage() == m_vk_texture->texture());
    }

    m_vk_texture->setT(float(static_cast<VulkanCustomTextureItem *>(m_item)->t()));
}

void VulkanCustomTextureNode::render()
{
    int currentFrameSlot = m_window->graphicsStateInfo().currentFrameSlot;

    QSGRendererInterface *rif = m_window->rendererInterface();
    VkCommandBuffer* cmdBuf = reinterpret_cast<VkCommandBuffer *>(rif->getResource(m_window, QSGRendererInterface::CommandListResource));

    m_vk_texture->render(cmdBuf,currentFrameSlot,this->m_width,this->m_height);
}

