#include "dpiscaleeventfilter.h"

namespace prism::qt::ui {

DPIScaleEventFilter::DPIScaleEventFilter(QObject *parent)
    : QObject{parent}
{

}

bool DPIScaleEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::WinIdChange)
    {
        // 窗口 DPI 缩放变化事件发生时执行的代码
        QWindow* window = dynamic_cast<QWindow*>(obj);
        if (window)
        {
            qreal dpiScale = window->screen()->devicePixelRatio();

            // 在这里处理 DPI 缩放变化，例如重新布局窗口内容
            qDebug() << "DPI Scale Changed: " << dpiScale;
        }
    }

    return QObject::eventFilter(obj, event);
}

}// namespace prism::qt::ui
