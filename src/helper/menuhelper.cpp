#include "menuhelper.h"
#include <QObject>
#include <private/qobject_p.h>
#include <QtQuickTemplates2/6.8.3/QtQuickTemplates2/private/qquickpopup_p.h>


namespace prism::qt::ui {

MenuHelper::MenuHelper(QObject* parent)
    : QObject{parent}
{
}



QQuickWindow* MenuHelper::setMenuStyle(QVariant menu, int xoffset, bool drawLine)
{
    QObject* menuObj = menu.value<QObject*>();
    if (!menuObj)
        return nullptr;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // ------- Qt5旧实现 -------
    auto d = QObjectPrivate::get(menuObj);
    QObjectPrivate::ConnectionData* cd = d->connections.loadRelaxed();
    if (cd && cd->senders)
    {
        for (QObjectPrivate::Connection* s = cd->senders; s; s = s->next)
        {
            QByteArray slotName{};
            if (!s->isSlotObject)
            {
                const QMetaMethod slot = menuObj->metaObject()->method(s->method());
                slotName = slot.methodSignature();
            }
            if (slotName == "clearPopupWindow()")
            {
                QQuickWindow* win = qobject_cast<QQuickWindow*>(s->sender);
                if (win)
                {
                    win->setColor("transparent");
                    win->setFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | win->flags());
                    win->setGeometry(win->x() - xoffset, win->y() - xoffset / 2,
                                     win->contentItem()->width(), win->contentItem()->height());
                    win->setOpacity(0);
                    if (drawLine)
                        QMetaObject::invokeMethod(menuObj, "drawLine", Qt::QueuedConnection);
                    return win;
                }
            }
        }
    }
    return nullptr;
#else
    // ------- Qt6新实现 -------
    auto popup = qobject_cast<QQuickPopup*>(menuObj);
    if (!popup)
        return nullptr;

    QQuickWindow* win = popup->popupItem()->window();
    if (!win)
        return nullptr;

    win->setColor(Qt::transparent);
    win->setFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | win->flags());
    win->setGeometry(win->x() - xoffset,
                     win->y() - xoffset / 2,
                     win->contentItem()->width(),
                     win->contentItem()->height());
    win->setOpacity(0.0);

    if (drawLine)
        QMetaObject::invokeMethod(menuObj, "drawLine", Qt::QueuedConnection);

    return win;
#endif
}


}// namespace prism::qt::ui
