#ifndef PRISM_QT_UI_CPP_UTILITY_H
#define PRISM_QT_UI_CPP_UTILITY_H

#include "../prismQt_ui_global.h"
#include <QObject>
#include <QUrl>
#include <QVariant>
#include <memory>
#include <QPoint>
#include <QTimer>
#include <QDateTime>
#include <QModelIndex>
#include <prism/utilities/typeName.hpp>


class QQuickWindow;
namespace prism::qt::ui {

class PRISMQT_UI_EXPORT cpp_utility : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString transThis READ transThis WRITE setTransThis NOTIFY transThisChanged)
  public:
    explicit cpp_utility(QObject* parent = nullptr);
    ~cpp_utility();
    static std::string openGLVersion ;
    bool is_ui_hang();
    const QString &transThis() const;
    void setTransThis(const QString &newTransThis);

public slots:
    void setCppOwnership(QObject *obj);
    void setQmlOwnership(QObject *obj);
    void enableWindowBorderless(QQuickWindow* win);
    void clearQmlCache();
    bool enableHotReload();
    std::optional<bool> bool2opt(bool b);
    QUrl transUrl(QString url);
    QString qurl2localfile(QUrl url);
    std::string stdstr(QString str);
    QString qstr(std::string str);
    double itof(int i);
    QPoint getMousePos();
    void forceUpdateMouseAreaHovered(QObject* item);

    QModelIndex getInvalidQModelIndex();

    std::shared_ptr<bool> showQmlDialog(QString qmlUrl,QObject* viewModel);
    void showQmlWindow(QString qmlUrl, QObject* viewModel =nullptr);

    bool load_language_qm(QString filePath);

    void setCursor(Qt::CursorShape cs);
    void restoreCursor();
    bool isqmllive();
    void openPath(const QString path);
    QObject* getModelIndexInternalPointer(const QModelIndex& item);

private:
    std::unique_ptr<QTimer> uiTimer_ ;
    int uiTimer_interval_ = 50 ;
    std::time_t pre_timepoint_ ;
    bool isqmllive_ = false;

    QString m_transThis;

signals:
    void transThisChanged();
};

}// namespace prism::qt::ui


#ifdef USING_PCL
extern "C"
{
PRISMQT_UI_EXPORT void initVTK();
}
#endif


#endif // PRISM_QT_UI_CPP_UTILITY_H
