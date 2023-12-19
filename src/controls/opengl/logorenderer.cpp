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
#include <QPainter>
#include <QPaintEngine>
#include <qmath.h>
#include <QDebug>
#include <mutex>
#include <iostream>

namespace prism::qt::ui{

LogoRenderer::LogoRenderer()
{
}

LogoRenderer::~LogoRenderer()
{
}


void LogoRenderer::paintQtLogo()
{
    constexpr static const GLfloat squareVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,

    };
    constexpr static const GLfloat coordVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };

    QOpenGLShaderProgram* program;
    if(this->frame.pixelType == prism::qt::ui::ENUM_PixelType::mono8)
        program = &program_mono8;
    else if(this->frame.pixelType == prism::qt::ui::ENUM_PixelType::rgb8)
        program = &program_rgb;
    else
        program = &program_mono8;


    program->enableAttributeArray(vertexInAL);
    program->setAttributeArray(vertexInAL,GL_FLOAT,squareVertices,2);

    program->enableAttributeArray(textureInAL);
    program->setAttributeArray(textureInAL,GL_FLOAT, coordVertices,2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textuniformID);
    // 设置纹理参数，包括过滤方式和包装方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if(this->frame.pixelType == prism::qt::ui::ENUM_PixelType::mono8)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width, texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.buffer.get());
    else  if(this->frame.pixelType == prism::qt::ui::ENUM_PixelType::rgb8)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.buffer.get());
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width, texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.buffer.get());

    glUniform1i(textuniformUL, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    program->disableAttributeArray(vertexInAL);

    program->disableAttributeArray(textureInAL);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qDebug() << "OpenGL Error:" << error;
    }

}


void LogoRenderer::initialize()
{
    if(!inited_)
    {
        initializeOpenGLFunctions();

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

        const char *vsrc1 =
                "attribute vec4 vertexIn;\n"
                "attribute vec2 textureIn;\n"
                "varying vec2 textureOut;\n"
                "uniform mediump mat4 matrix;\n"
                "void main(void)\n"
                "{\n"
                "    gl_Position = matrix * vertexIn;\n"
                "    textureOut = vec2(textureIn.x,1.0 -textureIn.y);"
                "}\n";

        const char *fsrc_nono8 =
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



        const char *fsrc_rgb =
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
    }
    QOpenGLShaderProgram* program;

    if(this->frame.pixelType == prism::qt::ui::ENUM_PixelType::mono8)
        program = &program_mono8;
    else if(this->frame.pixelType == prism::qt::ui::ENUM_PixelType::rgb8)
        program = &program_rgb;
    else
        program = &program_mono8;

    vertexInAL = program->attributeLocation("vertexIn");
    textureInAL = program->attributeLocation("textureIn");
    matrixUL = program->uniformLocation("matrix");

    if(!inited_)
    {
        // 创建纹理对象
        glGenTextures(1, &textuniformID);
        glBindTexture(GL_TEXTURE_2D, textuniformID);

        // 设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }

    // 获取纹理位置
    textuniformUL = glGetUniformLocation(program->programId(), "tex_");

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
    if(m_sn.empty())
        return;
    //glDepthMask(true);

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT );


    ////GL_CW 顺时针为正面
    ////GL_CCW 逆时针为正面，opengl默认选项
    glFrontFace(GL_CCW);
    ////GL_FRONT剔除正面
    ////GL_BACK剔除反面
    //glCullFace(GL_BACK);
    ////启用剔除面
    //glEnable(GL_CULL_FACE);
    ////启用深度测试
    //glEnable(GL_DEPTH_TEST);

    //

    QMatrix4x4 modelview;
    modelview.scale(m_fxScale,m_fyScale);

    QOpenGLShaderProgram* program;

    if(this->frame.pixelType == prism::qt::ui::ENUM_PixelType::mono8)
        program = &program_mono8;
    else if(this->frame.pixelType == prism::qt::ui::ENUM_PixelType::rgb8)
        program = &program_rgb;
    else
        program = &program_mono8;

    program->bind();
    program->setUniformValue(matrixUL, modelview);
    paintQtLogo();
    program->release();

    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);

}

}// namespace prism::qt::ui
