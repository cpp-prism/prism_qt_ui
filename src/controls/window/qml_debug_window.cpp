#include "qml_debug_window.h"

namespace prism::qt::ui{
qmlDebugWindow::qmlDebugWindow(QWindow* parent):QQuickWindow(parent)
{

}

QString qmlDebugWindow::loadUrl() const
{
    return m_loadUrl;
}


QVariant qmlDebugWindow::dialogResult() const
{
    return m_dialogResult;
}

QVariant qmlDebugWindow::viewModel() const
{
    return m_viewModel;
}



void qmlDebugWindow::setLoadUrl(QString loadUrl)
{
    if (m_loadUrl == loadUrl)
        return;

    m_loadUrl = loadUrl;
    emit loadUrlChanged(m_loadUrl);
}


void qmlDebugWindow::setDialogResult(QVariant dialogResult)
{
    if(dialogResult.isNull() || dialogResult.canConvert(QVariant::Bool))
    {
        m_dialogResult = dialogResult;
        emit dialogResultChanged(m_dialogResult);
        this->close();
    }
}

void qmlDebugWindow::setViewModel(QVariant viewModel)
{
    if (m_viewModel == viewModel)
        return;

    m_viewModel = viewModel;
    emit viewModelChanged(m_viewModel);
}

}//namespace prism::qt::ui

