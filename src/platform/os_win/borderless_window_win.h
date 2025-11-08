#ifndef PRISM_QT_UI_BORDERLESS_WINDOW_WIN_H
#define PRISM_QT_UI_BORDERLESS_WINDOW_WIN_H

#include <QObject>
#include <QQuickWindow>
#include <QOperatingSystemVersion>
#include "include/prism/qt/ui/prismQt_ui_global.h"

namespace prism::qt::ui {

class PRISMQT_UI_EXPORT borderless_window_win : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(int titleHeight READ titleHeight WRITE setTitleHeight NOTIFY titleHeightChanged)
    Q_PROPERTY(int titleBarBtnWidth READ titleBarBtnWidth WRITE setTitleBarBtnWidth NOTIFY titleBarBtnWidthChanged)


public slots:

    void maximize();
    void minimize();
    void restore();

private:
    int m_titleHeight = 30;
    int m_titleBarBtnWidth = 100;

public:
    explicit borderless_window_win(QQuickWindow* parent = nullptr);

    int titleHeight() const;
    void setTitleHeight(int newTitleHeight);

    int titleBarBtnWidth() const;
    void setTitleBarBtnWidth(int newTitleBarBtnWidth);

signals:
    void titleHeightChanged();
    void titleBarBtnWidthChanged();

protected:
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
};

} // namespace prism::qt::ui

#endif // PRISM_QT_UI_BORDERLESS_WINDOW_WIN_H
