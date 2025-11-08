#ifndef VULKANCUSTOMTEXTUREITEM_H
#define VULKANCUSTOMTEXTUREITEM_H

#include <QtQuick/QQuickItem>

#include "vulkanCustomTextureNode.h"

//! [1]
class VulkanCustomTextureItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    //QML_ELEMENT

public:
    VulkanCustomTextureItem();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private slots:
    void invalidateSceneGraph();

private:
    void releaseResources() override;

    VulkanCustomTextureNode *m_node = nullptr;
    qreal m_t = 0;
};
//! [1]

#endif // VULKANCUSTOMTEXTUREITEM_H
