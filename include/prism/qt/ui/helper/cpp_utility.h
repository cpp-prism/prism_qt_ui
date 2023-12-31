#ifndef PRISM_QT_UI_CPP_UTILITY_H
#define PRISM_QT_UI_CPP_UTILITY_H

#include "../prismQt_ui_global.h"
#include <QObject>
#include <QUrl>
#include <QVariant>
#include <memory>
#include <QPoint>


class QQuickWindow;
namespace prism::qt::ui {

class PRISMQT_UI_EXPORT cpp_utility : public QObject
{
    Q_OBJECT
  public:
    explicit cpp_utility(QObject* parent = nullptr);
  public slots:
    void setCppOwnership(QObject *obj);
    void setQmlOwnership(QObject *obj);
    void enableWindowBorderless(QQuickWindow* win);
    void clearQmlCache();
    bool enableHotReload();
    QUrl transUrl(QString url);
    std::string stdstr(QString str);
    QString qstr(std::string str);
    double itof(int i);
    QPoint getMousePos();

    std::shared_ptr<bool> showQmlDialog(QString qmlUrl,QObject* viewModel);
    void showQmlWindow(QString qmlUrl, QObject* viewModel =nullptr);

    bool load_language_qm(QString filePath);

    void setCursor(Qt::CursorShape cs);
    void restoreCursor();

  signals:
};

}// namespace prism::qt::ui


#ifdef USING_PCL
extern "C"
{
PRISMQT_UI_EXPORT void initVTK();
}
#endif


#endif // PRISM_QT_UI_CPP_UTILITY_H
