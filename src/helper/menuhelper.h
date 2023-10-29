#ifndef PRISM_QT_UI_MENUHELPER_H
#define PRISM_QT_UI_MENUHELPER_H

#include <QDebug>
#include <QObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QVariant>
#include <qglobal.h>

namespace prism::qt::ui {

class MenuHelper : public QObject
{
    Q_OBJECT

  public:
    explicit MenuHelper(QObject* parent = nullptr);
    Q_INVOKABLE QQuickWindow* setMenuStyle(QVariant menu, int xoffset, bool drawLine = true);
    Q_INVOKABLE void updateWin(QVariant win)
    {
        QQuickWindow* w = win.value<QQuickWindow*>();
        if (w)
            w->update();
    }

  private:
};
}//namespace prism::qt::ui

#endif // PRISM_QT_UI_MENUHELPER_H
