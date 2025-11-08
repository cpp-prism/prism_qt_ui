#include "vulkanCustomTextureItem.h"
#include  "vulkancustomtexturenode.h"



VulkanCustomTextureItem::VulkanCustomTextureItem()
{
    setFlag(ItemHasContents, true);
}

void VulkanCustomTextureItem::invalidateSceneGraph() // called on the render thread when the scenegraph is invalidated
{
    m_node = nullptr;
}

void VulkanCustomTextureItem::releaseResources() // called on the gui thread if the item is removed from scene
{
    m_node = nullptr;
}

QSGNode *VulkanCustomTextureItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    VulkanCustomTextureNode *n = static_cast<VulkanCustomTextureNode *>(node);

    if (!n && (width() <= 0 || height() <= 0))
        return nullptr;

    if (!n) {
        m_node = new VulkanCustomTextureNode(this);
        n = m_node;
    }

    m_node->sync();

    n->setTextureCoordinatesTransform(QSGSimpleTextureNode::NoTransform);
    n->setFiltering(QSGTexture::Linear);
    n->setRect(0, 0, width(), height());

    window()->update(); // ensure getting to beforeRendering() at some point

    return n;
}

void VulkanCustomTextureItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);

    if (newGeometry.size() != oldGeometry.size())
        update();
}

void VulkanCustomTextureItem::setT(qreal t)
{
    if (t == m_t)
        return;

    m_t = t;
    emit tChanged();

    update();
}


//#include "vulkanCustomTextureItem.moc"
