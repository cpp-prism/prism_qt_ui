#ifndef PRISM_QT_UI_BORDERLESS_WINDOW_WIN_H
#define PRISM_QT_UI_BORDERLESS_WINDOW_WIN_H

#include <QObject>
#include <QQuickWindow>

namespace prism::qt::ui {

class borderless_window_win : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(int titleHeight READ titleHeight WRITE setTitleHeight NOTIFY titleHeightChanged)
    Q_PROPERTY(int titleBarBtnWidth READ titleBarBtnWidth WRITE setTitleBarBtnWidth NOTIFY titleBarBtnWidthChanged)
  private:
    int m_titleHeight;
    int m_titleBarBtnWidth;

  public:
    explicit borderless_window_win(QQuickWindow* parent);
    borderless_window_win();

    int titleHeight() const;
    void setTitleHeight(int newTitleHeight);

    int titleBarBtnWidth() const;
    void setTitleBarBtnWidth(int newTitleBarBtnWidth);

  signals:
    void titleHeightChanged();

    void titleBarBtnWidthChanged();

  protected:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
};
}//namespace prism::qt::ui

#endif // PRISM_QT_UI_BORDERLESS_WINDOW_WIN_H
