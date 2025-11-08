#ifndef VULKANCUSTOMTEXTURENODE_H
#define VULKANCUSTOMTEXTURENODE_H

#include <QObject>
#include <QSGSimpleTextureNode>
#include <QSGTextureProvider>

class RSVulkanCustomTexture;
class VulkanCustomTexture;

class VulkanCustomTextureNode : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    VulkanCustomTextureNode(QQuickItem *item);
    ~VulkanCustomTextureNode() override;

    QSGTexture *texture() const override;

    void sync();


private slots:
    void render();

private:

    QQuickItem *m_item;
    QQuickWindow *m_window;
    int m_width = -1;
    int m_height = -1;
    qreal m_dpr;

    RSVulkanCustomTexture* m_vk_texture = nullptr;

};

#endif // VULKANCUSTOMTEXTURENODE_H
