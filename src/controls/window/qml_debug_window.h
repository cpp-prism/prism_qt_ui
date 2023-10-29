#ifndef PRISM_QT_UI_CONTROL_WINDOW_QMLDEBUGWINDOW_H
#define PRISM_QT_UI_CONTROL_WINDOW_QMLDEBUGWINDOW_H

#include <QObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QVariant>

namespace prism::qt::ui{
class qmlDebugWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(QString loadUrl READ loadUrl WRITE setLoadUrl NOTIFY loadUrlChanged)
    Q_PROPERTY(QVariant dialogResult READ dialogResult WRITE setDialogResult NOTIFY dialogResultChanged)
    Q_PROPERTY(QVariant viewModel READ viewModel WRITE setViewModel NOTIFY viewModelChanged)

    QString m_loadUrl;
    QVariant m_dialogResult ;
    QVariant m_viewModel;

public:
    qmlDebugWindow(QWindow* parent =nullptr);
    QString loadUrl() const;
    QVariant dialogResult() const;

    QVariant viewModel() const;

public slots:
void setLoadUrl(QString loadUrl);
void setDialogResult(QVariant dialogResult);
void setViewModel(QVariant viewModel);

signals:
void loadUrlChanged(QString loadUrl);
void dialogResultChanged(QVariant dialogResult);
void viewModelChanged(QVariant viewModel);
};

} //namespace prism::qt::ui
#endif // PRISM_QT_UI_CONTROL_WINDOW_QMLDEBUGWINDOW_H
