// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "rhi_video_render.h"
#include <QSGTextureProvider>
#include <QSGRenderNode>
#include <QSGTransformNode>
#include <QQuickWindow>
#include <QFile>

#include <rhi/qrhi.h>
#include "include/prism/qt/ui/img_buffer_Info.h"

QByteArray loadImgRaw(const QString &path, prism::qt::ui::ENUM_PixelType format, int width,  int stride,int height)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QByteArray buffer;
    qint64 bytesRead=0;

    switch (format) {
    case prism::qt::ui::ENUM_PixelType::mono8:
        buffer = QByteArray(stride * height, 0);
        bytesRead = file.read(buffer.data(), buffer.size());
        break;

    case prism::qt::ui::ENUM_PixelType::bgr24:
    case prism::qt::ui::ENUM_PixelType::rgb24:
        buffer = QByteArray(stride * height * 3, 0);
        bytesRead = file.read(buffer.data(), buffer.size());
        break;

    case prism::qt::ui::ENUM_PixelType::bgra32:
    case prism::qt::ui::ENUM_PixelType::rgba32:
        buffer = QByteArray(stride * height * 4, 0);
        bytesRead = file.read(buffer.data(), buffer.size());
        break;

    case prism::qt::ui::ENUM_PixelType::nv12:
    case prism::qt::ui::ENUM_PixelType::yuv420p:
        buffer = QByteArray(stride * height * 1.5, 0);
        bytesRead = file.read(buffer.data(), buffer.size());
        break;

    default:
        break;
    }

    if (bytesRead != buffer.size()) {
        qWarning("Mono8 raw file size mismatch");
        return {};
    }

    return buffer;
}

void bgr24_to_bgra32(const QByteArray& rgb, QByteArray& rgba,  int s, int w, int h)
{

    rgba.resize(s * h * 4);

    int srcStride = s * 3;   // 可能 > width * 3
    int dstStride = w * 4;

    rgba.resize(dstStride * h);

    for (int y = 0; y < h; ++y) {
        const uchar* srcLine =
            reinterpret_cast<const uchar*>(rgb.constData()) + y * srcStride;
        uchar* dstLine =
            reinterpret_cast<uchar*>(rgba.data()) + y * dstStride;

        for (int x = 0; x < w; ++x) {
            dstLine[x*4 + 0] = srcLine[x*3 + 0];
            dstLine[x*4 + 1] = srcLine[x*3 + 1];
            dstLine[x*4 + 2] = srcLine[x*3 + 2];
            dstLine[x*4 + 3] = 255;
        }
    }

}

//![node]
class RhiVideoRenderNode : public QSGRenderNode
{
public:
    RhiVideoRenderNode(QQuickWindow *window);

    void setVertices(const QList<QVector4D> &vertices);

    void prepare() override;
    void render(const RenderState *state) override;
    void releaseResources() override;
    RenderingFlags flags() const override;
    QSGRenderNode::StateFlags changedStates() const override;

    void set_video_width(int width);
    void set_video_stride(int stride);
    void set_video_height(int height);
    void set_video_format(prism::qt::ui::ENUM_PixelType format);
    void set_video_data(void *data);

protected:
    QQuickWindow *m_window;
    std::unique_ptr<QRhiBuffer> m_vertexBuffer;
    std::unique_ptr<QRhiBuffer> m_uniformBuffer;
    std::unique_ptr<QRhiShaderResourceBindings> m_resourceBindings;
    std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
    QRhiShaderStage m_shader_vert;
    QRhiShaderStage m_shader_frag_mono8;
    QRhiShaderStage m_shader_frag_rgb24;
    QRhiShaderStage m_shader_frag_rgba32;
    QRhiShaderStage m_shader_frag_bgr24;
    QRhiShaderStage m_shader_frag_bgra32;
    QRhiShaderStage m_shader_frag_nv12;
    QRhiShaderStage m_shader_frag_yuv420p;

    QList<QRhiShaderStage> m_shaders;

    bool m_verticesDirty = true;
    QList<QVector4D> m_vertices;

    prism::qt::ui::ENUM_PixelType m_video_format = prism::qt::ui::ENUM_PixelType::mono8;
    int m_video_stride = 300;
    int m_video_width = 300;
    int m_video_height = 168;
    bool m_texture_dirty =true;
    void* m_video_data = nullptr;
    bool m_needUploadData = true;

    std::unique_ptr<QRhiTexture> m_texture_1;
    std::unique_ptr<QRhiSampler> m_sampler_1;

    std::unique_ptr<QRhiTexture> m_texture_2;
    std::unique_ptr<QRhiSampler> m_sampler_2;

    std::unique_ptr<QRhiTexture> m_texture_3;
    std::unique_ptr<QRhiSampler> m_sampler_3;
};
void RhiVideoRenderNode::set_video_data(void *data)
{
    m_video_data = data;
}

void RhiVideoRenderNode::set_video_format( prism::qt::ui::ENUM_PixelType format)
{
    m_video_format = format;
}

void RhiVideoRenderNode::set_video_height(int height)
{
    if(height != m_video_height)
        m_texture_dirty = true;
    m_video_height = height;
}

void RhiVideoRenderNode::set_video_stride(int stride)
{
    if(stride != m_video_stride)
        m_texture_dirty = true;
    m_video_stride = stride;
}

void RhiVideoRenderNode::set_video_width(int width)
{
    if(width != m_video_width)
        m_texture_dirty = true;
    m_video_width = width;
}

//![node]

RhiVideoRenderNode::RhiVideoRenderNode(QQuickWindow *window)
    : m_window(window)
{
    QFile file;
    file.setFileName(":/prism_qt_ui/Shaders/qsb/image_2d.vert.qsb");
    if (!file.open(QFile::ReadOnly))
        qFatal("Failed to load vertex shader");
    m_shader_vert =  QRhiShaderStage(QRhiShaderStage::Vertex, QShader::fromSerialized(file.readAll()));
    file.close();

    file.setFileName(":/prism_qt_ui/Shaders/qsb/mono8.frag.qsb");
    if (!file.open(QFile::ReadOnly))
        qFatal("Failed to load fragment shader");
    m_shader_frag_mono8 = QRhiShaderStage(QRhiShaderStage::Fragment, QShader::fromSerialized(file.readAll()));
    file.close();

    file.setFileName(":/prism_qt_ui/Shaders/qsb/rgb24.frag.qsb");
    if (!file.open(QFile::ReadOnly))
        qFatal("Failed to load fragment shader");
    m_shader_frag_rgb24 = QRhiShaderStage(QRhiShaderStage::Fragment, QShader::fromSerialized(file.readAll()));
    file.close();

    file.setFileName(":/prism_qt_ui/Shaders/qsb/rgba32.frag.qsb");
    if (!file.open(QFile::ReadOnly))
        qFatal("Failed to load fragment shader");
    m_shader_frag_rgba32 = QRhiShaderStage(QRhiShaderStage::Fragment, QShader::fromSerialized(file.readAll()));
    file.close();

    file.setFileName(":/prism_qt_ui/Shaders/qsb/bgr24.frag.qsb");
    if (!file.open(QFile::ReadOnly))
        qFatal("Failed to load fragment shader");
    m_shader_frag_bgr24 = QRhiShaderStage(QRhiShaderStage::Fragment, QShader::fromSerialized(file.readAll()));
    file.close();

    file.setFileName(":/prism_qt_ui/Shaders/qsb/bgra32.frag.qsb");
    if (!file.open(QFile::ReadOnly))
        qFatal("Failed to load fragment shader");
    m_shader_frag_bgra32 = QRhiShaderStage(QRhiShaderStage::Fragment, QShader::fromSerialized(file.readAll()));
    file.close();

    file.setFileName(":/prism_qt_ui/Shaders/qsb/nv12.frag.qsb");
    if (!file.open(QFile::ReadOnly))
        qFatal("Failed to load fragment shader");
    m_shader_frag_nv12 = QRhiShaderStage(QRhiShaderStage::Fragment, QShader::fromSerialized(file.readAll()));
    file.close();

    file.setFileName(":/prism_qt_ui/Shaders/qsb/yuv420p.frag.qsb");
    if (!file.open(QFile::ReadOnly))
        qFatal("Failed to load fragment shader");
    m_shader_frag_yuv420p = QRhiShaderStage(QRhiShaderStage::Fragment, QShader::fromSerialized(file.readAll()));
    file.close();


}

void RhiVideoRenderNode::setVertices(const QList<QVector4D> &vertices)
{
    if (m_vertices == vertices)
        return;

    m_verticesDirty = true;
    m_vertices = vertices;

    markDirty(QSGNode::DirtyGeometry);
}

//![node-release]
void RhiVideoRenderNode::releaseResources()
{
    m_vertexBuffer.reset();
    m_uniformBuffer.reset();
    m_pipeline.reset();
    m_resourceBindings.reset();
    m_sampler_1.reset();
    m_sampler_2.reset();
    m_sampler_3.reset();
    m_texture_1.reset();
    m_texture_2.reset();
    m_texture_3.reset();
}
//![node-release]

//![node-flags]
QSGRenderNode::RenderingFlags RhiVideoRenderNode::flags() const
{
    // We are rendering 2D content directly into the scene graph using QRhi, no
    // direct usage of a 3D API. Hence NoExternalRendering. This is a minor
    // optimization.

    // Additionally, the node takes the item transform into account by relying
    // on projectionMatrix() and matrix() (see prepare()) and never rendering at
    // other Z coordinates. Hence DepthAwareRendering. This is a potentially
    // bigger optimization.

    return QSGRenderNode::NoExternalRendering | QSGRenderNode::DepthAwareRendering;
}
//![node-flags]

QSGRenderNode::StateFlags RhiVideoRenderNode::changedStates() const
{
    // In Qt 6 only ViewportState and ScissorState matter, the rest is ignored.
    return QSGRenderNode::StateFlag::ViewportState | QSGRenderNode::StateFlag::CullState | QSGRenderNode::ScissorState  ;
}

//![node-prepare]
void RhiVideoRenderNode::prepare()
{
    if(!m_video_data)
        return;
    QRhi *rhi = m_window->rhi();
    QRhiResourceUpdateBatch *resourceUpdates = rhi->nextResourceUpdateBatch();

    if(!m_texture_1 || m_texture_dirty)
    {
        switch (m_video_format) {
        case prism::qt::ui::ENUM_PixelType::mono8:
        case prism::qt::ui::ENUM_PixelType::nv12:
        case prism::qt::ui::ENUM_PixelType::yuv420p:
            m_texture_1.reset(rhi->newTexture(QRhiTexture::R8, QSize(m_video_stride, m_video_height), 1));
            break;
        case prism::qt::ui::ENUM_PixelType::rgba32:
            m_texture_1.reset(rhi->newTexture(QRhiTexture::RGBA8, QSize(m_video_stride, m_video_height), 1));
            break;

        case prism::qt::ui::ENUM_PixelType::bgra32:
            m_texture_1.reset(rhi->newTexture(QRhiTexture::BGRA8, QSize(m_video_stride, m_video_height), 1));
            break;

        default:
            break;
        }

        if (!m_texture_1->create()) {
            qWarning("Failed to create texture");
            return;
        }
        if (!m_sampler_1) {
            m_sampler_1.reset( rhi->newSampler(  QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None, QRhiSampler::ClampToEdge,QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge ));
            m_sampler_1->create();
        }
    }
    if( (!m_texture_2 || m_texture_dirty) &&( prism::qt::ui::ENUM_PixelType::nv12 == m_video_format || prism::qt::ui::ENUM_PixelType::yuv420p == m_video_format ))
    {
        if (prism::qt::ui::ENUM_PixelType::nv12 ==m_video_format)
            m_texture_2.reset(rhi->newTexture(QRhiTexture::RG8, QSize(m_video_stride/2, m_video_height/2), 1));
        else
            m_texture_2.reset(rhi->newTexture(QRhiTexture::R8, QSize(m_video_stride/2, m_video_height/2), 1));

        if (!m_texture_2->create()) {
            qWarning("Failed to create texture");
            return;
        }
        if (!m_sampler_2)
        {
            m_sampler_2.reset(rhi->newSampler( QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None, QRhiSampler::ClampToEdge,QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge ) );
            m_sampler_2->create();
        }
    }
    if((!m_texture_3 || m_texture_dirty) && prism::qt::ui::ENUM_PixelType::yuv420p == m_video_format )
    {
        m_texture_3.reset(rhi->newTexture(QRhiTexture::R8, QSize(m_video_stride/2, m_video_height/2), 1));

        if (!m_texture_3->create()) {
            qWarning("Failed to create texture");
            return;
        }
        if (!m_sampler_3)
        {
            m_sampler_3.reset(rhi->newSampler( QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None, QRhiSampler::ClampToEdge,QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge ) );
            m_sampler_3->create();
        }
    }




    //顶点
    if (m_verticesDirty) {
        m_vertexBuffer.reset();
        m_verticesDirty = false;
    }
    if (!m_vertexBuffer) {
        m_vertexBuffer.reset(rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer,
                                            m_vertices.count() * sizeof(QVector4D)));
        m_vertexBuffer->create();
        resourceUpdates->uploadStaticBuffer(m_vertexBuffer.get(), m_vertices.constData());
    }
    // rotation matrix , uscale, vsclae, simplers
    if (!m_uniformBuffer) {
        m_uniformBuffer.reset(rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 64+8));
        m_uniformBuffer->create();
    }
    if (!m_resourceBindings || m_texture_dirty) {
        m_resourceBindings.reset(rhi->newShaderResourceBindings());
        QList<QRhiShaderResourceBinding> shaderRbs;
        // binding = 0 : uniform buffer
        shaderRbs.append( QRhiShaderResourceBinding::uniformBuffer( 0, QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage, m_uniformBuffer.get() ));

        // binding = 1 : sampler2D tex_1
        shaderRbs.append( QRhiShaderResourceBinding::sampledTexture( 1, QRhiShaderResourceBinding::FragmentStage, m_texture_1.get(), m_sampler_1.get() ));

        // binding = 2 : sampler2D tex_2
        if(prism::qt::ui::ENUM_PixelType::nv12 == m_video_format || prism::qt::ui::ENUM_PixelType::yuv420p == m_video_format)
            shaderRbs.append( QRhiShaderResourceBinding::sampledTexture( 2, QRhiShaderResourceBinding::FragmentStage, m_texture_2.get(), m_sampler_2.get() ));

        // binding = 3 : sampler2D tex_3
        if(prism::qt::ui::ENUM_PixelType::yuv420p == m_video_format)
            shaderRbs.append( QRhiShaderResourceBinding::sampledTexture( 3, QRhiShaderResourceBinding::FragmentStage, m_texture_3.get(), m_sampler_3.get() ));

        m_resourceBindings->setBindings(shaderRbs.cbegin(),shaderRbs.constEnd());
        m_resourceBindings->create();
    }

    if (!m_pipeline || m_texture_dirty) {
        m_pipeline.reset(rhi->newGraphicsPipeline());

        // If layer.enabled == true on our QQuickItem, the rendering face is flipped for
        // backends with isYUpInFrameBuffer == true (OpenGL). This does not happen with
        // RHI backends with isYUpInFrameBuffer == false. We swap the triangle winding
        // order to work around this.
        m_pipeline->setFrontFace(renderTarget()->resourceType() == QRhiResource::TextureRenderTarget
                                         && rhi->isYUpInFramebuffer()
                                     ? QRhiGraphicsPipeline::CCW
                                     : QRhiGraphicsPipeline::CW);
        m_pipeline->setCullMode(QRhiGraphicsPipeline::Back);
        m_pipeline->setTopology(QRhiGraphicsPipeline::TriangleStrip);
        QRhiGraphicsPipeline::TargetBlend blend;
        blend.enable = true;
        m_pipeline->setTargetBlends({ blend });
        m_pipeline->setShaderResourceBindings(m_resourceBindings.get());
        m_shaders.clear();
        m_shaders.append(this->m_shader_vert);
        switch (m_video_format) {
        case prism::qt::ui::ENUM_PixelType::mono8:
            m_shaders.append(this->m_shader_frag_mono8);
            break;
        case prism::qt::ui::ENUM_PixelType::bgra32:
            m_shaders.append(this->m_shader_frag_bgra32);
            break;
        case prism::qt::ui::ENUM_PixelType::rgba32:
            m_shaders.append(this->m_shader_frag_rgba32);
            break;
        case prism::qt::ui::ENUM_PixelType::bgr24:
            m_shaders.append(this->m_shader_frag_bgr24);
            break;
        case prism::qt::ui::ENUM_PixelType::rgb24:
            m_shaders.append(this->m_shader_frag_rgb24);
            break;
        case prism::qt::ui::ENUM_PixelType::nv12:
            m_shaders.append(this->m_shader_frag_nv12);
            break;
        case prism::qt::ui::ENUM_PixelType::yuv420p:
            m_shaders.append(this->m_shader_frag_yuv420p);
            break;
        default:
            break;
        }
        m_pipeline->setShaderStages(m_shaders.cbegin(), m_shaders.cend());
        m_pipeline->setDepthTest(true);
        QRhiVertexInputLayout inputLayout;
        inputLayout.setBindings({ { 4 * sizeof(float) } }); //x,y, u, v
        inputLayout.setAttributes({
            // location = 0 -> vertex
            { 0, 0, QRhiVertexInputAttribute::Float2, 0 },

            // location = 1 -> texCoord
            { 0, 1, QRhiVertexInputAttribute::Float2, 2 * sizeof(float) }//the 4th param for skip x,y
        });
        m_pipeline->setVertexInputLayout(inputLayout);

        m_pipeline->setRenderPassDescriptor(renderTarget()->renderPassDescriptor());


        m_pipeline->create();

        //(!m_shaders.last().shader().isValid())
        //	qDebug() << "Fragment shader error:" << m_shaders.last().shader().stage();
    }

    m_texture_dirty = false;


    QMatrix4x4 mvp = *projectionMatrix() * *matrix();
    double itemW = m_vertices[3].x();
    double itemH = m_vertices[3].y();
    double cx = itemW * 0.5;
    double cy = itemH * 0.5;
    mvp.translate(cx, cy);
    double scaleX = 1.0;
    double scaleY = 1.0;
    double height = m_video_height * itemW / m_video_width;
    if (height <= itemH) {
        scaleY = height / itemH;
    } else {
        double width = m_video_width * itemH / m_video_height;
        scaleX = width / itemW;
    }
    mvp.scale(scaleX, scaleY);
    mvp.translate(-cx, -cy);
    resourceUpdates->updateDynamicBuffer(m_uniformBuffer.get(), 0, 64, mvp.constData());
    float u_scale = m_video_width * 1.0 / m_video_stride;
    resourceUpdates->updateDynamicBuffer(m_uniformBuffer.get(), 64, 4, &u_scale);
    float v_scale = 1.0;
    resourceUpdates->updateDynamicBuffer(m_uniformBuffer.get(), 68, 4, &v_scale);


    int texture_data_size_1 =0;
    int texture_data_size_2 =0;
    int texture_data_size_3 =0;
    switch (m_video_format) {
    case prism::qt::ui::ENUM_PixelType::mono8:
        texture_data_size_1 = m_video_stride * m_video_height;
        break;
    case prism::qt::ui::ENUM_PixelType::nv12:
        texture_data_size_1 = m_video_stride * m_video_height;
        texture_data_size_2 = m_video_stride/2.0 * m_video_height/2.0 * 2.0 ;
        break;
    case prism::qt::ui::ENUM_PixelType::yuv420p:
        texture_data_size_1 = m_video_stride * m_video_height;
        texture_data_size_2 = m_video_stride * 1.0 /2.0 * m_video_height/2.0;
        texture_data_size_3 = m_video_stride * 1.0 /2.0 * m_video_height/2.0;
        break;
    case prism::qt::ui::ENUM_PixelType::bgr24:
    case prism::qt::ui::ENUM_PixelType::rgb24:
        texture_data_size_1 = m_video_stride * m_video_height*3;
        break;
    case prism::qt::ui::ENUM_PixelType::bgra32:
    case prism::qt::ui::ENUM_PixelType::rgba32:
        texture_data_size_1 = m_video_stride * m_video_height*4;
        break;
    default:
        break;
    }
    if(!m_needUploadData)
    {
        m_needUploadData = true;
        return;
    }

    resourceUpdates->uploadTexture(
        m_texture_1.get(),
        QRhiTextureUploadDescription( QRhiTextureUploadEntry(0,0,QRhiTextureSubresourceUploadDescription(m_video_data,texture_data_size_1))));

    if(prism::qt::ui::ENUM_PixelType::nv12 == m_video_format ||
        prism::qt::ui::ENUM_PixelType::yuv420p == m_video_format)
    {
        resourceUpdates->uploadTexture(
            m_texture_2.get(),
            QRhiTextureUploadDescription( QRhiTextureUploadEntry(0,0,
                                                                QRhiTextureSubresourceUploadDescription(reinterpret_cast<uint8_t*>(m_video_data) + texture_data_size_1,
                                                                                                        texture_data_size_2 ))));
    }
    if(prism::qt::ui::ENUM_PixelType::yuv420p == m_video_format)
    {
        resourceUpdates->uploadTexture(
            m_texture_3.get(),
            QRhiTextureUploadDescription( QRhiTextureUploadEntry(0,0,
                                                                QRhiTextureSubresourceUploadDescription(reinterpret_cast<uint8_t*>(m_video_data) + texture_data_size_1 +texture_data_size_2 ,
                                                                                                        texture_data_size_3 ))));
    }

    commandBuffer()->resourceUpdate(resourceUpdates);
}

//![node-render]
void RhiVideoRenderNode::render(const RenderState * state)
{
    if(!m_video_data)
        return;
    QRhiCommandBuffer *cb = commandBuffer();

    cb->setGraphicsPipeline(m_pipeline.get());
    QSize renderTargetSize = renderTarget()->pixelSize();
    cb->setViewport(QRhiViewport(0, 0, renderTargetSize.width(), renderTargetSize.height()));

    if (state->scissorEnabled()) {
        const QRect r = state->scissorRect();
        cb->setScissor(QRhiScissor(
            r.x(),
            r.y(),
            r.width(),
            r.height()
        ));
    }

    cb->setShaderResources();
    QRhiCommandBuffer::VertexInput vertexBindings[] = { { m_vertexBuffer.get(), 0 } };
    cb->setVertexInput(0, 1, vertexBindings);
    cb->draw(m_vertices.count());

}
//![node-render]

//![item-ctor]
RhiVideoRender::RhiVideoRender(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
    setFrameInfo(new prism::qt::core::prismModelProxy<prism::qt::ui::ImgFrameInfo>());
    connect(this, &RhiVideoRender::verticesChanged, this, &RhiVideoRender::update);
}
//![item-ctor]

QList<QVector4D> RhiVideoRender::vertices() const
{
    return m_vertices;
}

void RhiVideoRender::setVertices(const QList<QVector4D> &newVertices)
{
    if (m_vertices == newVertices)
        return;

    m_vertices = newVertices;
    emit verticesChanged();
}

//![item-update]
QSGNode *RhiVideoRender::updatePaintNode(QSGNode *old, UpdatePaintNodeData *)
{
    RhiVideoRenderNode *node = static_cast<RhiVideoRenderNode *>(old);

    if (!node)
        node = new RhiVideoRenderNode(window());

    auto q = prism::qt::ui::ImgFrameQueue::getQueue(m_sn);

    //mock data
//    if(!q->size() )
//    {
//        prism::qt::ui::ImgFrameInfo frameinfo;
//        std::shared_ptr<QByteArray> data  = std::make_shared<QByteArray>();
//        QByteArray ba;
//        QString path;
////#define PIXEL_TYPE  mono8
////#define PIXEL_TYPE  rgb24
////define PIXEL_TYPE  bgr24
////#define PIXEL_TYPE  nv12
////#define PIXEL_TYPE  yuv420p
////#define PIXEL_TYPE  rgba32
//#define PIXEL_TYPE  bgra32
//        if(m_sn == "0")
//        {
//            frameinfo.width = 300;
//            frameinfo.height = 168;
//            frameinfo.stride = 300;
//            path = QString("raw_out/1_300_168_300_%1.raw").arg(STR(bgra32));
//
//            prism::qt::ui::ENUM_PixelType format = prism::qt::ui::ENUM_PixelType::bgra32;
//            frameinfo.pixelType = format;
//            ba = loadImgRaw(path, format,frameinfo.width, frameinfo.stride, frameinfo.height);
//        }
//        else if(m_sn == "1")
//        {
//            frameinfo.width = 237;
//            frameinfo.height = 213;
//            frameinfo.stride = 238;
//            path = QString("raw_out/2_237_213_238_%1.raw").arg(STR(yuv420p));
//            prism::qt::ui::ENUM_PixelType format = prism::qt::ui::ENUM_PixelType::yuv420p;
//            frameinfo.pixelType = format;
//            ba = loadImgRaw(path, format,frameinfo.width, frameinfo.stride, frameinfo.height);
//        }
//        else if(m_sn == "2")
//        {
//            frameinfo.width = 187;
//            frameinfo.height = 270;
//            frameinfo.stride = 188;
//            path = QString("raw_out/3_187_270_188_%1.raw").arg(STR(nv12));
//            prism::qt::ui::ENUM_PixelType format = prism::qt::ui::ENUM_PixelType::nv12;
//            frameinfo.pixelType = format;
//            ba = loadImgRaw(path, format,frameinfo.width, frameinfo.stride, frameinfo.height);
//        }
//        else if(m_sn == "3")
//        {
//            frameinfo.width = 189;
//            frameinfo.height = 267;
//            frameinfo.stride = 190;
//            path = QString("raw_out/4_189_267_190_%1.raw").arg(STR(mono8));
//            prism::qt::ui::ENUM_PixelType format = prism::qt::ui::ENUM_PixelType::mono8;
//            frameinfo.pixelType = format;
//            ba = loadImgRaw(path, format,frameinfo.width, frameinfo.stride, frameinfo.height);
//        }
//        switch (frameinfo.pixelType) {
//        case prism::qt::ui::ENUM_PixelType::mono8:
//        case prism::qt::ui::ENUM_PixelType::nv12:
//        case prism::qt::ui::ENUM_PixelType::yuv420p:
//        case prism::qt::ui::ENUM_PixelType::bgra32:
//        case prism::qt::ui::ENUM_PixelType::rgba32:
//            *data  = ba;
//            break;
//        case prism::qt::ui::ENUM_PixelType::bgr24:
//            bgr24_to_bgra32(ba,*data,frameinfo.stride,frameinfo.width,frameinfo.height);
//            frameinfo.pixelType =  prism::qt::ui::ENUM_PixelType::bgra32;
//            frameinfo.stride =  frameinfo.width ;
//            break;
//        case prism::qt::ui::ENUM_PixelType::rgb24:
//            bgr24_to_bgra32(ba,*data,frameinfo.stride,frameinfo.width,frameinfo.height);
//            frameinfo.pixelType =  prism::qt::ui::ENUM_PixelType::rgba32;
//            frameinfo.stride =  frameinfo.width ;
//            break;
//        default:
//            break;
//        }
//        frameinfo.buffer_handel = std::static_pointer_cast<void>(data);
//        frameinfo.buffer = (void*)data.get()->constData();
//        q->push(frameinfo);
//
//    }
    //popup data
    prism::qt::ui::ImgFrameInfo pop_info;
    if(q->tryPopLatest(pop_info))
    {
        *this->frameInfo()->instance() = std::move(pop_info);
        this->frameInfo()->update();
        node->set_video_width(pop_info.width);
        node->set_video_stride(pop_info.stride);
        node->set_video_height(pop_info.height);
        node->set_video_format(pop_info.pixelType);
        node->set_video_data(pop_info.buffer);
    }


    node->setVertices(m_vertices);

    return node;
}
//![item-update]

std::string RhiVideoRender::sn() const
{
    return m_sn;
}

void RhiVideoRender::setSn(const std::string &newSn)
{
    if (m_sn == newSn)
        return;
    m_sn = newSn;
    emit snChanged();
}

prism::qt::core::prismModelProxy<prism::qt::ui::ImgFrameInfo> *RhiVideoRender::frameInfo() const
{
    return m_frameInfo;
}

void RhiVideoRender::setFrameInfo(prism::qt::core::prismModelProxy<prism::qt::ui::ImgFrameInfo> *newFrameInfo)
{
    if (m_frameInfo == newFrameInfo)
        return;
    m_frameInfo = newFrameInfo;
    emit frameInfoChanged();
}
