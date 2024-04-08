/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PRISM_QT_UI_THREADRENDERER_H
#define PRISM_QT_UI_THREADRENDERER_H

#include <QQuickItem>
#include <string>
#include <QTimer>
#include <memory>

namespace prism::qt::ui{
class RenderThread;

class ThreadRenderer : public QQuickItem
{
    Q_OBJECT
    //QML_NAMED_ELEMENT(Renderer)
    //为了有机会渲染后另存为图片，可以选择false
    Q_PROPERTY(bool releaseBuferAfterRender READ releaseBuferAfterRender WRITE setReleaseBuferAfterRender NOTIFY releaseBuferAfterRenderChanged)

public:
    ThreadRenderer();
    ~ThreadRenderer();

    static QList<QThread *> threads;
    std::string sn();

    bool releaseBuferAfterRender() const;
    void setReleaseBuferAfterRender(bool newReleaseBuferAfterRender);

public Q_SLOTS:
    void ready();
    void setCamSn(QString sn);
    void prismSizeChanged();
    void prismSizeChangedDetect();


signals:
    void sizeChanged();
    void releaseBuferAfterRenderChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

private:
    RenderThread *m_renderThread;
    std::string m_sn;
    QMetaObject::Connection m_shutdown_connection;
    bool m_sizechangedFlag = false;
    std::unique_ptr<QTimer> m_sizechangedTimer = std::make_unique<QTimer>();
    bool m_releaseBuferAfterRender = true;
};
}// namespace prism::qt::ui

#endif //PRISM_QT_UI_THREADRENDERER_H
