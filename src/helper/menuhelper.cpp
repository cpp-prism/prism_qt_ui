#include "menuhelper.h"
#include <QObject>
#include <private/qobject_p.h>
#include <QRegExp>

namespace prism::qt::ui {

MenuHelper::MenuHelper(QObject* parent)
    : QObject{parent}
{
}



QQuickWindow* MenuHelper::setMenuStyle(QVariant menu, int xoffset, bool drawLine)
{
    QQuickWindow* win = nullptr;
    QObject* menuObje = menu.value<QObject*>();
    {
        auto d = QObjectPrivate::get(menuObje);
        QObjectPrivate::ConnectionData* cd = d->connections.loadRelaxed();
        if (cd && cd->senders)
        {
            for (QObjectPrivate::Connection* s = cd->senders; s; s = s->next)
            {
                QByteArray slotName{};
                if (!s->isSlotObject)
                {
                    const QMetaMethod slot = menuObje->metaObject()->method(s->method());
                    slotName = slot.methodSignature();
                }
                if (slotName == "clearPopupWindow()")
                {
                    win = qobject_cast<QQuickWindow*>(s->sender);
                    win->setColor("transparent");
                    win->setFlags((Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | win->flags()));
                    win->setGeometry(win->x() - xoffset, win->y() - xoffset / 2, win->contentItem()->width(), win->contentItem()->height());
                    win->setOpacity(0);
                    if (drawLine)
                        QMetaObject::invokeMethod(menuObje, "drawLine", Qt::QueuedConnection);
                    break;
                }
            }
        }
    }
    //    if (win)
    //    {
    //        auto d = QObjectPrivate::get(win);
    //        QObjectPrivate::ConnectionData* cd = d->connections.loadRelaxed();
    //        if (cd && cd->senders)
    //        {
    //            for (QObjectPrivate::Connection* s = cd->senders; s; s = s->next)
    //            {
    //                QByteArray slotName{};
    //                if (!s->isSlotObject)
    //                {
    //                    const QMetaMethod slot = win->metaObject()->method(s->method());
    //                    slotName = slot.methodSignature();
    //                }
    //                if (slotName == "updateSize()")
    //                {
    //                    QObject* qobj = qobject_cast<QObject*>(s->sender);
    //                    //                    qDebug() << qobj;
    //                    //                    qobj->disconnect(win);
    //                    break;
    //                }
    //            }
    //        }
    //        win->setGeometry(win->x() - xoffset, win->y() - xoffset / 2, win->contentItem()->width(), win->contentItem()->height());
    //        return win;
    //    }
    return win;
}

}// namespace prism::qt::ui
