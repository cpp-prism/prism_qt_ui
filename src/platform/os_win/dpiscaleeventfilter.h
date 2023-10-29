#ifndef PRISM_QT_UI_DPISCALEEVENTFILTER_H
#define PRISM_QT_UI_DPISCALEEVENTFILTER_H


#include <QCoreApplication>
#include <QEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>
#include <QDebug>

namespace prism::qt::ui {

class DPIScaleEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit DPIScaleEventFilter(QObject* parent = nullptr) ;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

}// namespace prism::qt::ui
#endif // PRISM_QT_UI_DPISCALEEVENTFILTER_H
