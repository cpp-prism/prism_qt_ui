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

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "threadrenderer.h"
#include "logorenderer.h"

#include <QtCore/QMutex>
#include <QtCore/QThread>

#include <QtGui/QGuiApplication>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <mutex>
#include <prism/container.hpp>
#include <prism/qt/core/helper/qeventloopguard.h>
#include <prism/qt/core/prismQt_events.h>
#include <qsgsimpletexturenode.h>
#include <string>

namespace prism::qt::ui
{
QList<QThread*> ThreadRenderer::threads;

/*
 * The render thread shares a context with the scene graph and will
 * render into two separate FBOs, one to use for display and one
 * to use for rendering
 */
class RenderThread : public QThread
{
    Q_OBJECT
  public:
    RenderThread(const QSize& size, ThreadRenderer* parent_render)
        : surface(nullptr), context(nullptr), m_renderFbo(nullptr), m_displayFbo(nullptr), m_logoRenderer(nullptr), m_size(size), mp_render(parent_render)
    {
        ThreadRenderer::threads << this;
    }

    QOffscreenSurface* surface;
    QOpenGLContext* context;

    void setSn(const std::string& sn)
    {
        m_sn = sn;
    }
    std::string sn()
    {
        return m_sn;
    }

    void setReleaseBuferAfterRender(bool newReleaseBuferAfterRender);

  public slots:
    void renderNext()
    {
        if (m_sn.empty())
            return;
        if (mp_render->sn().empty())
            return;
        context->makeCurrent(surface);

        bool isFirstTime = false;
        if (!m_renderFbo)
        {
            isFirstTime = true;
            // Initialize the buffers and renderer
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            m_renderFbo = new QOpenGLFramebufferObject(m_size, format);
            m_displayFbo = new QOpenGLFramebufferObject(m_size, format);
            m_logoRenderer = new LogoRenderer();
            m_logoRenderer->setCamSn(m_sn);
            m_logoRenderer->isFirstFrame_ = isFirstTime;
            m_logoRenderer->setReleaseBuferAfterRender(m_releaseBuferAfterRender);
            m_logoRenderer->initialize();
        }

        if (!isFirstTime)
        {
            std::shared_ptr<img_buffer_Info> buf = img_buffer_Info::infos[m_sn];
            if (!buf)
                buf = img_buffer_Info::infos[m_sn] = std::make_shared<img_buffer_Info>();
            if (!isInitColor)
            {
                isInitColor = true;
                emit mp_render->inited_background();
            }
            {
                std::unique_lock<std::mutex> lk(buf->buffer_mux);

                buf->buffer_cv.wait(lk);

                if (buf->doFreeOpenGL)
                {
                    buf->doFreeOpenGL = false;
                    buf->frames.clear();
                    shutDown();
                    return;
                }

                if (!buf->frames.size())
                    return;
                m_logoRenderer->frame = buf->frames.back();
                // buf->frames.pop_back();
                buf->frames.clear();
            }

            m_size.setWidth(mp_render->width());
            m_size.setHeight(mp_render->height());

            if (std::abs(m_displayFbo->width() - m_size.width()) > 0.01 || std::abs(m_displayFbo->height() - m_size.height()) > 0.01 || std::abs(m_renderFbo->width() - m_size.width()) > 0.01 || std::abs(m_renderFbo->height() - m_size.height()) > 0.01)
            {
                {
                    auto* tmp = new QOpenGLFramebufferObject(m_size, m_displayFbo->format());
                    delete m_displayFbo;
                    m_displayFbo = tmp;
                }
                {
                    auto* tmp = new QOpenGLFramebufferObject(m_size, m_renderFbo->format());
                    delete m_renderFbo;
                    m_renderFbo = tmp;
                }
            }

            if (std::abs(m_logoRenderer->frame.width - m_logoRenderer->texture_width) > 0.01 ||
                std::abs(m_logoRenderer->frame.height - m_logoRenderer->texture_height) > 0.01)
            {
                m_logoRenderer->texture_width = m_logoRenderer->frame.width;
                m_logoRenderer->texture_height = m_logoRenderer->frame.height;

                emit mp_render->frameSizeChanged(m_logoRenderer->texture_width,
                                                 m_logoRenderer->texture_height);
            }

            if (m_logoRenderer->frame.pixelType != buf->pre_frame.pixelType)
            {
                m_logoRenderer->initialize();
            }
            buf->pre_frame = m_logoRenderer->frame;

            //如果纹理宽不变保持比例放大到和控件一样，纹理高<=控件高
            double height = m_logoRenderer->texture_height * mp_render->width() / m_logoRenderer->texture_width;
            if (height <= mp_render->height())
            {
                m_logoRenderer->m_fxScale = 1;
                m_logoRenderer->m_fyScale = height / mp_render->height();
            }
            else //否则纹理高保持不变，纹理宽缩小
            {
                double width = m_logoRenderer->texture_width * mp_render->height() / m_logoRenderer->texture_height;
                m_logoRenderer->m_fxScale = width / mp_render->width();
                m_logoRenderer->m_fyScale = 1;
            }
        }

        m_renderFbo->bind();
        context->functions()->glViewport(0, 0, m_size.width(), m_size.height());

        m_logoRenderer->render();

        // We need to flush the contents to the FBO before posting
        // the texture to the other thread, otherwise, we might
        // get unexpected results.
        context->functions()->glFlush();

        m_renderFbo->bindDefault();

        qSwap(m_renderFbo, m_displayFbo);
        emit textureReady(m_displayFbo->texture(), m_size);
        // if (isFirstTime)
        //{
        //     // this->mp_render->update();
        // }
    }

    void shutDown()
    {
        qDebug() << "render thread shutdown";
        context->makeCurrent(surface);
        delete m_renderFbo;
        delete m_displayFbo;
        delete m_logoRenderer;
        context->doneCurrent();
        delete context;

        // schedule this to be deleted only after we're done cleaning up
        surface->deleteLater();

        // Stop event processing, move the thread to GUI and make sure it is deleted.
        exit();
        moveToThread(QGuiApplication::instance()->thread());
    }

  signals:
    void textureReady(int id, const QSize& size);

  private:
    QOpenGLFramebufferObject* m_renderFbo;
    QOpenGLFramebufferObject* m_displayFbo;

    LogoRenderer* m_logoRenderer;
    QSize m_size;
    std::string m_sn;
    ThreadRenderer* mp_render = nullptr;
    bool m_releaseBuferAfterRender = true;
    bool isInitColor = false;
};

class TextureNode : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

  public:
    TextureNode(QQuickWindow* window)
        : m_id(0), m_size(0, 0), m_texture(nullptr), m_window(window)
    {
        // Our texture node must have a texture, so use the default 0 texture.
        m_texture = m_window->createTextureFromId(0, QSize(1, 1));
        setTexture(m_texture);
        setFiltering(QSGTexture::Linear);
    }

    ~TextureNode() override
    {
        delete m_texture;
    }

  signals:
    void textureInUse();
    void pendingNewTexture();

  public slots:

    // This function gets called on the FBO rendering thread and will store the
    // texture id and size and schedule an update on the window.
    void newTexture(int id, const QSize& size)
    {
        m_mutex.lock();
        m_id = id;
        m_size = size;
        m_mutex.unlock();

        // We cannot call QQuickWindow::update directly here, as this is only allowed
        // from the rendering thread or GUI thread.
        emit pendingNewTexture();
    }

    // Before the scene graph starts to render, we update to the pending texture
    void prepareNode()
    {
        m_mutex.lock();
        int newId = m_id;
        QSize size = m_size;
        m_id = 0;
        m_mutex.unlock();
        if (newId)
        {
            delete m_texture;
            // note: include QQuickWindow::TextureHasAlphaChannel if the rendered content
            // has alpha.
            m_texture = m_window->createTextureFromId(newId, size);
            setTexture(m_texture);

            markDirty(DirtyMaterial);

            // This will notify the rendering thread that the texture is now being rendered
            // and it can start rendering to the other one.
            emit textureInUse();
        }
    }

  private:
    int m_id;
    QSize m_size;

    QMutex m_mutex;

    QSGTexture* m_texture;
    QQuickWindow* m_window;
};

ThreadRenderer::ThreadRenderer()
    : m_renderThread(nullptr)
{
    setFlag(ItemHasContents, true);
    m_renderThread = new RenderThread(QSize(512, 512), this);

    connect(this, &ThreadRenderer::widthChanged, this, &ThreadRenderer::prismSizeChanged);
    connect(this, &ThreadRenderer::heightChanged, this, &ThreadRenderer::prismSizeChanged);

    m_sizechangedTimer->setInterval(200);
    connect(m_sizechangedTimer.get(), &QTimer::timeout, this, &ThreadRenderer::prismSizeChangedDetect);
}

ThreadRenderer::~ThreadRenderer()
{
    std::shared_ptr<img_buffer_Info> buf = img_buffer_Info::infos[m_sn];
    if (buf)
    {
        buf->doFreeOpenGL = true;
        buf->buffer_cv.notify_one();
        // if (buf->pre_frame.buffer_handel)
        //     buf->pre_frame.buffer_handel.reset();
    }
}

void ThreadRenderer::ready()
{
    if (this->sn().empty())
        return;
    m_renderThread->surface = new QOffscreenSurface();
    m_renderThread->surface->setFormat(m_renderThread->context->format());
    m_renderThread->surface->create();

    m_renderThread->moveToThread(m_renderThread);

    // connect(window(), &QQuickWindow::sceneGraphInvalidated, m_renderThread, &RenderThread::shutDown, Qt::QueuedConnection);

    m_renderThread->start();
    update();
}

std::string ThreadRenderer::sn()
{
    return m_sn;
}

void ThreadRenderer::setCamSn(QString sn)
{
    m_sn = sn.toStdString();
    if (m_renderThread)
        m_renderThread->setSn(m_sn);
}

void ThreadRenderer::prismSizeChanged()
{
    if (m_sn.empty())
        return;

    m_sizechangedFlag = true;
    m_sizechangedTimer->stop();
    m_sizechangedTimer->setSingleShot(true);
    m_sizechangedTimer->start();
}

void ThreadRenderer::prismSizeChangedDetect()
{
    if (m_sn.empty())
        return;

    if (!m_sizechangedFlag)
        return;
    m_sizechangedFlag = false;

    std::shared_ptr<img_buffer_Info> buf = img_buffer_Info::infos[m_sn];
    if (!buf)
        buf = img_buffer_Info::infos[m_sn] = std::make_shared<img_buffer_Info>();
    {
        std::unique_lock<std::mutex> lk(buf->buffer_mux);

        if (buf->doFreeOpenGL)
            return;

        buf->frames.clear();
        if (buf->pre_frame.buffer)
        {
            buf->frames.push_back(buf->pre_frame);
            buf->buffer_cv.notify_one();
        }
    }
}

QSGNode* ThreadRenderer::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    TextureNode* node = static_cast<TextureNode*>(oldNode);

    if (!m_renderThread->context)
    {
        QOpenGLContext* current = window()->openglContext();
        // Some GL implementations requres that the currently bound context is
        // made non-current before we set up sharing, so we doneCurrent here
        // and makeCurrent down below while setting up our own context.
        current->doneCurrent();

        m_renderThread->context = new QOpenGLContext();
        m_renderThread->context->setFormat(current->format());
        m_renderThread->context->setShareContext(current);
        m_renderThread->context->create();
        m_renderThread->context->moveToThread(m_renderThread);

        current->makeCurrent(window());

        QMetaObject::invokeMethod(this, "ready");
        return nullptr;
    }

    if (!node)
    {
        node = new TextureNode(window());

        /* Set up connections to get the production of FBO textures in sync with vsync on the
         * rendering thread.
         *
         * When a new texture is ready on the rendering thread, we use a direct connection to
         * the texture node to let it know a new texture can be used. The node will then
         * emit pendingNewTexture which we bind to QQuickWindow::update to schedule a redraw.
         *
         * When the scene graph starts rendering the next frame, the prepareNode() function
         * is used to update the node with the new texture. Once it completes, it emits
         * textureInUse() which we connect to the FBO rendering thread's renderNext() to have
         * it start producing content into its current "back buffer".
         *
         * This FBO rendering pipeline is throttled by vsync on the scene graph rendering thread.
         */
        connect(m_renderThread, &RenderThread::textureReady, node, &TextureNode::newTexture, Qt::DirectConnection);
        connect(node, &TextureNode::pendingNewTexture, window(), &QQuickWindow::update, Qt::QueuedConnection);
        connect(window(), &QQuickWindow::beforeRendering, node, &TextureNode::prepareNode, Qt::DirectConnection);
        connect(node, &TextureNode::textureInUse, m_renderThread, &RenderThread::renderNext, Qt::QueuedConnection);

        // Get the production of FBO textures started..
        QMetaObject::invokeMethod(m_renderThread, "renderNext", Qt::QueuedConnection);
    }

    node->setRect(boundingRect());

    return node;
}

bool ThreadRenderer::releaseBuferAfterRender() const
{
    return m_releaseBuferAfterRender;
}

void ThreadRenderer::setReleaseBuferAfterRender(bool newReleaseBuferAfterRender)
{
    m_releaseBuferAfterRender = newReleaseBuferAfterRender;
    m_renderThread->setReleaseBuferAfterRender(newReleaseBuferAfterRender);
    if (m_releaseBuferAfterRender != newReleaseBuferAfterRender)
    {
        emit releaseBuferAfterRenderChanged();
    }
}

void RenderThread::setReleaseBuferAfterRender(bool newReleaseBuferAfterRender)
{
    m_releaseBuferAfterRender = newReleaseBuferAfterRender;
    if (m_logoRenderer)
        m_logoRenderer->setReleaseBuferAfterRender(newReleaseBuferAfterRender);
}

} // namespace prism::qt::ui
#include "threadrenderer.moc"
