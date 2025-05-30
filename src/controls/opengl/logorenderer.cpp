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

#include "logorenderer.h"
#include <QDebug>
#include <QPaintEngine>
#include <QPainter>
#include <iostream>
#include <mutex>
#include <qmath.h>


namespace prism::qt::ui
{

LogoRenderer::LogoRenderer()
{
}

LogoRenderer::~LogoRenderer()
{
}

void LogoRenderer::paintQtLogo()
{

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textuniformID);

    if (!isFirstFrame_)
    {
        if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::mono8)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width, texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.buffer);
        else if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::rgb8)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.buffer);
        else if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::bgr8)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.buffer);
        else if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::yuv420)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width, texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.buffer);
        else if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::yuv420p)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width, texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.buffer);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width, texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.buffer);

        glUniform1i(textuniformUL, 0);

        if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::yuv420p)
        {
            //u
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textuniformUID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width/2, texture_height/2, 0, GL_RED, GL_UNSIGNED_BYTE, (u_int8_t*)frame.buffer + texture_width*texture_height);
            glUniform1i(textuniformUUL, 1);

            //v

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, textuniformVID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width/2, texture_height/2, 0, GL_RED, GL_UNSIGNED_BYTE, (u_int8_t*)frame.buffer + texture_width*texture_height  * 5/4 );
            glUniform1i(textuniformVUL, 2);
        }
        else if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::yuv420)
        {
            //uv

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textuniformUID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, texture_width/2, texture_height/2, 0, GL_RG, GL_UNSIGNED_BYTE, (u_int8_t*)frame.buffer + texture_width*texture_height);
            glUniform1i(textuniformUUL, 1);
        }


    }
    else
    {
        isFirstFrame_ = false;
        GLsizei width = 640;  // 纹理的宽度
        GLsizei height = 480; // 纹理的高度
        GLubyte r = 204 ;    // 红色通道值 (0-255)
        GLubyte g = 204;    // 绿色通道值 (0-255)
        GLubyte b = 204;    // 蓝色通道值 (0-255)

        // 创建一个大小为 (width * height * 3) 的数组来存储颜色数据
        std::vector<GLubyte> textureData(width * height * 3);

        // 填充数组，每个像素都是相同的颜色
        for (int i = 0; i < width * height * 3; i += 3)
        {
            textureData[i] = r;     // 红色
            textureData[i + 1] = g; // 绿色
            textureData[i + 2] = b; // 蓝色
        }

        // 上传纹理数据
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData.data());
        glUniform1i(textuniformUL, 0);
    }



    if (m_releaseBuferAfterRender)
    {
        frame.buffer_handel.reset();
        frame.buffer = nullptr;
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

bool LogoRenderer::releaseBuferAfterRender() const
{
    return m_releaseBuferAfterRender;
}

void LogoRenderer::setReleaseBuferAfterRender(bool newReleaseBuferAfterRender)
{
    m_releaseBuferAfterRender = newReleaseBuferAfterRender;
}

void LogoRenderer::initialize()
{
    if (!inited_)
    {
        initializeOpenGLFunctions();

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

        const char* vsrc1 =
#ifdef USING_OPENGLES
            "precision mediump float;\n"
#endif
            "attribute vec4 vertexIn;\n"
            "attribute vec2 textureIn;\n"
            "varying vec2 textureOut;\n"
            "uniform mediump mat4 matrix;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = matrix * vertexIn;\n"
            "    textureOut = vec2(textureIn.x,1.0 -textureIn.y);"
            "}\n";

        const char* fsrc_nono8 =
#ifdef USING_OPENGLES
            "precision mediump float;\n"
#endif
            "varying vec2 textureOut;\n"
            "uniform sampler2D tex_;\n"
            "void main(void)\n"
            "{\n"
            "    float mono8 = texture2D(tex_, textureOut).r ; \n"
            "    gl_FragColor = vec4(mono8,mono8,mono8, 1.0);\n"
            "}\n";
        program_mono8.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc1);
        program_mono8.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc_nono8);
        program_mono8.link();

        const char* fsrc_rgb =
#ifdef USING_OPENGLES
            "precision mediump float;\n"
#endif
            "varying vec2 textureOut;\n"
            "uniform sampler2D tex_;\n"
            "void main(void)\n"
            "{\n"
            "    vec3 rgb_ = texture2D(tex_, textureOut).rgb ; \n"
            "    gl_FragColor = vec4(rgb_,1.0);\n"
            "}\n";
        program_rgb.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc1);
        program_rgb.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc_rgb);
        program_rgb.link();

        const char* fsrc_bgr =
#ifdef USING_OPENGLES
            "precision mediump float;\n"
#endif
            "varying vec2 textureOut;\n"
            "uniform sampler2D tex_;\n"
            "void main(void)\n"
            "{\n"
            "    vec3 rgb_ = texture2D(tex_, textureOut).rgb ; \n"
            "    gl_FragColor = vec4(rgb_.b, rgb_.g, rgb_.r,1.0);\n"
            "}\n";
        program_bgr.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc1);
        program_bgr.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc_bgr);
        program_bgr.link();

            //{ 1.0,   0.0, 1.402, 1.0,   -0.344, -0.714, 1.0,   1.772, 0.0 }, // JPEG
            //{ 1.164, 0.0, 1.596, 1.164, -0.392, -0.813, 1.164, 2.017, 0.0 }, // SMPTE170M(BT.601)
            //{ 1.164, 0.0, 1.793, 1.164, -0.213, -0.533, 1.164, 2.112, 0.0 }, // Rec709
            //{ 1, 1, 1, 0, -0.39465,  2.03211, 1.13983, -0.58060,  0}
        const char* fsrc_yuv420p =
        #ifdef USING_OPENGLES
            "precision mediump  float;\n"
        #endif
        "varying vec2 textureOut;"
        "uniform sampler2D tex_;"
        "uniform sampler2D tex_u;"
        "uniform sampler2D tex_v;"
        "const mat3 yuv2rgb = mat3(  "
        "  1, 1, 1, 0, -0.39465,  2.03211, 1.13983, -0.58060,  0 "
        ");  "
        "void main(void)"
        "{"
        "    vec3 yuv;"
        "    vec3 rgb;    "
        "    yuv.x = texture2D(tex_, textureOut).r ;"
        "    yuv.y = texture2D(tex_u, textureOut).r  - 0.5;"
        "    yuv.z = texture2D(tex_v, textureOut).r  - 0.5;"
        "    rgb = yuv2rgb * yuv;"
        "    gl_FragColor = vec4(rgb, 1.0);"
        "}";
        program_yuv420p.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc1);
        program_yuv420p.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc_yuv420p);
        program_yuv420p.link();

        const char* fsrc_yuv420 =
        #ifdef USING_OPENGLES
            "precision mediump float;\n"
        #endif
            "varying vec2 textureOut;"
            "uniform sampler2D tex_;"
            "uniform sampler2D tex_u;"
            "const mat3 yuv2rgb = mat3("
            "  1.164383,  0.000000,  1.596000,"
            "  1.164383, -0.391762, -0.812968,"
            "  1.164383,  2.017232,  0.000000"
            ");"

            "void main(void)"
            "{"
            "    vec3 yuv;"
            "    vec3 rgb;"
            "    yuv.x = texture2D(tex_, textureOut).r;"
            "    yuv.y = texture2D(tex_u, textureOut).r - 0.5;"
            "    yuv.z = texture2D(tex_u, textureOut).g - 0.5;"
            "    rgb = yuv2rgb * yuv;"
            "    gl_FragColor = vec4(clamp(rgb,0.0,1.0), 1.0);"
            "}";


        program_yuv420.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc1);
        program_yuv420.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc_yuv420);
        program_yuv420.link();


        // 创建纹理对象
        glGenTextures(1, &textuniformID);
        glBindTexture(GL_TEXTURE_2D, textuniformID);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


        glGenTextures(1, &textuniformUID);
        glBindTexture(GL_TEXTURE_2D, textuniformUID);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenTextures(1, &textuniformVID);
        glBindTexture(GL_TEXTURE_2D, textuniformVID);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


        // Fixes abnormality with 174x100 yuv data
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //yuv use 1   rgb use defalt 4.
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);


        //多重采样 抗锯齿
        // glEnable(GL_MULTISAMPLE);
    }


    m_fAngle = 0;
    m_fxScale = 1;
    m_fyScale = 1;
    inited_ = true;
}

void LogoRenderer::setCamSn(const std::string& sn)
{
    m_sn = sn;
}

void LogoRenderer::render()
{
    if (m_sn.empty())
        return;

    // glDepthMask(true);

    if (isFirstFrame_)
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    else
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    //多重采样 抗锯齿
    // glEnable(GL_MULTISAMPLE);

    ////GL_CW 顺时针为正面
    ////GL_CCW 逆时针为正面，opengl默认选项
    //glFrontFace(GL_CCW);
    ////GL_FRONT剔除正面
    ////GL_BACK剔除反面
    //glCullFace(GL_BACK);
    ////启用剔除面
    // glEnable(GL_CULL_FACE);
    ////启用深度测试
    // glEnable(GL_DEPTH_TEST);


    if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::mono8)
        program = &program_mono8;
    else if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::rgb8)
        program = &program_rgb;
    else if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::bgr8)
        program = &program_bgr;
    else if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::yuv420)
        program = &program_yuv420;
    else if (this->frame.pixelType == prism::qt::ui::ENUM_PixelType::yuv420p)
        program = &program_yuv420p;
    else
        program = &program_mono8;


    program->bind();

    vertexInAL = program->attributeLocation("vertexIn");
    textureInAL = program->attributeLocation("textureIn");
    matrixUL = program->uniformLocation("matrix");


    // 获取纹理位置
    textuniformUL = glGetUniformLocation(program->programId(), "tex_");
    textuniformUUL = glGetUniformLocation(program->programId(), "tex_u");
    textuniformVUL = glGetUniformLocation(program->programId(), "tex_v");

    constexpr static const GLfloat squareVertices[] = { -1.0f, -1.0f,
                                                        1.0f, -1.0f,
                                                        -1.0f, 1.0f,
                                                        1.0f, 1.0f,  };

    constexpr static const GLfloat coordVertices[] = { 0.0f, 1.0f,
                                                       1.0f, 1.0f,
                                                       0.0f, 0.0f,
                                                       1.0f, 0.0f, };

    QMatrix4x4 modelview;
    modelview.scale(m_fxScale, m_fyScale);


    program->setUniformValue(matrixUL, modelview);

    program->enableAttributeArray(vertexInAL);
    program->setAttributeArray(vertexInAL, GL_FLOAT, squareVertices, 2);

    program->enableAttributeArray(textureInAL);
    program->setAttributeArray(textureInAL, GL_FLOAT, coordVertices, 2);

    paintQtLogo();

    program->disableAttributeArray(vertexInAL);
    program->disableAttributeArray(textureInAL);

    glBindTexture(GL_TEXTURE_2D, 0);
    program->release();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        qDebug() << "OpenGL Error:" << error;
    }

    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_CULL_FACE);
}

} // namespace prism::qt::ui
