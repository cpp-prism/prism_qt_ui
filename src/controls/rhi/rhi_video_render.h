// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CUSTOMRENDER_H
#define CUSTOMRENDER_H

#include "include/prism/qt/ui/img_buffer_Info.h"
#include <QQuickItem>
#include <QVector4D>
#include <prism/qt/core/hpp/prismQt.hpp>

//![item]
class RhiVideoRender : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QList<QVector4D> vertices READ vertices WRITE setVertices NOTIFY verticesChanged)
    Q_PROPERTY(std::string sn READ sn WRITE setSn NOTIFY snChanged FINAL)
    Q_PROPERTY(prism::qt::core::prismModelProxy<prism::qt::ui::ImgFrameInfo>* frameInfo READ frameInfo WRITE setFrameInfo NOTIFY frameInfoChanged FINAL)


public:
    explicit RhiVideoRender(QQuickItem *parent = nullptr);

    QList<QVector4D> vertices() const;
    void setVertices(const QList<QVector4D> &newVertices);


    std::string sn() const;
    void setSn(const std::string &newSn);

    prism::qt::core::prismModelProxy<prism::qt::ui::ImgFrameInfo> *frameInfo() const;
    void setFrameInfo(prism::qt::core::prismModelProxy<prism::qt::ui::ImgFrameInfo> *newFrameInfo);

signals:
    void verticesChanged();


    void snChanged();

    void frameInfoChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *old, UpdatePaintNodeData *) override;

private:
    QList<QVector4D> m_vertices;
    std::string m_sn;
    prism::qt::core::prismModelProxy<prism::qt::ui::ImgFrameInfo> *m_frameInfo = nullptr;
};
//![item]

#endif // CUSTOMRENDER_H
