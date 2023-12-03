#include "include/prism/qt/ui/helper/cpp_utility.h"
#include "src/controls/window/qml_debug_window.h"
#include "../platform/i_borderless_window_helper.h"
#include <prism/container.hpp>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

namespace prism::qt::ui {
cpp_utility::cpp_utility(QObject* parent)
    : QObject(parent)
{
}
void cpp_utility::setQmlOwnership(QObject *obj)
{
    if(QQmlEngine::objectOwnership(obj) != QQmlEngine::JavaScriptOwnership)
    {
        QQmlEngine::setObjectOwnership(obj,QQmlEngine::ObjectOwnership::JavaScriptOwnership);
    }

}

void cpp_utility::setCppOwnership(QObject *obj)
{
    if(QQmlEngine::objectOwnership(obj) != QQmlEngine::CppOwnership)
    {
        QQmlEngine::setObjectOwnership(obj,QQmlEngine::ObjectOwnership::CppOwnership);
    }

}

void cpp_utility::enableWindowBorderless(QQuickWindow* win)
{
    auto h = Container::get()->resolve_object<i_borderless_window_helper>();
    if (h != nullptr)
        h->enableWindowBorderless(win->winId());
}

void cpp_utility::clearQmlCache()
{
    std::shared_ptr<QQmlApplicationEngine> engine = Container::get()->resolve_object<QQmlApplicationEngine>();
    engine->clearComponentCache();
}

bool cpp_utility::enableHotReload()
{
    static int qml_live_flag = -1;
    if (qml_live_flag == -1)
    {
        int length = QCoreApplication::arguments().length();
        QString arg1;
        if (length > 1)
            arg1 = QCoreApplication::arguments().at(1);
        if (length > 1 && arg1 == "qml_live")
            qml_live_flag = 1;
        else
            qml_live_flag = 0;
    }
    return qml_live_flag;
}

QUrl cpp_utility::transUrl(QString url)
{
    if (enableHotReload())
    {
        if (url.isEmpty())
            return QUrl();
        QString root(PRISMQT_SOLUTION_SOURCEDIR);
        QString relativePath ;
        if(url.startsWith("qrc:/prism_qt_ui"))
            relativePath = "";
        else
            relativePath = "../";
        auto rx = QRegExp(QString::fromStdString(R"(^qrc:/(\w+)(.*))"));
        rx.lastIndexIn(url);
        QString url_replaced = QString("file:///%1/%2%3/src/%4%5").arg(root).arg(relativePath).arg(rx.cap(1)).arg(rx.cap(1)).arg(rx.cap(2));
        //qDebug()<< "trans url :" << url_replaced;
        return QUrl(url_replaced);
    }
    return url;
}

std::string cpp_utility::stdstr(QString str)
{
    return str.toStdString();
}

QString cpp_utility::qstr(std::string str)
{
    return QString::fromStdString(str);
}

double cpp_utility::itof(int i)
{
    return i;
}

QPoint cpp_utility::getMousePos()
{
    return  QCursor::pos();
}


std::shared_ptr<bool> cpp_utility::showQmlDialog(QString qmlUrl,QObject* viewModel)
{
    std::shared_ptr<bool> result ;
    std::shared_ptr<QQmlApplicationEngine> p_engine = Container::get()->resolve_object<QQmlApplicationEngine>();
    QQmlComponent component(p_engine.get());

    QQuickWindow::setDefaultAlphaBuffer(true);
    auto url = transUrl("qrc:/prism_qt_ui/window/QmlDebugWindow.qml");
    component.loadUrl(url);
    qmlDebugWindow* win = nullptr;
    if( component.isReady())
    {
        QEventLoop loop;
        auto a =   component.create();
        win =  reinterpret_cast<qmlDebugWindow*>(a);
        win->setViewModel(QVariant::fromValue(viewModel));
        QObject::connect(win,&QQuickWindow::destroyed,&loop,[&](auto* p){
            Q_UNUSED(p)
            loop.exit();
        });

        win->setLoadUrl(qmlUrl);
        loop.exec();

        //#undef Bool
        //if(win->dialogResult().canConvert(QVariant::Bool))
        //{
        //    result = std::make_shared<bool>(win->dialogResult().value<bool>());
        //}

        //loop.disconnect(win);

        //if(win)
        //    win->deleteLater();
    }
    else
    {
        qWarning() << component.errorString();
        throw component.errorString().toStdString().c_str();
    }

    return result;

}

void cpp_utility::showQmlWindow(QString qmlUrl,QObject* viewModel)
{
    QQuickWindow::setDefaultAlphaBuffer(true);

    std::shared_ptr<QQmlApplicationEngine> p_engine = Container::get()->resolve_object<QQmlApplicationEngine>();
    std::shared_ptr<QQuickWindow> mainWindow = prism::Container::get()->resolve_object<QQuickWindow>();
    QQmlComponent component(p_engine.get(),mainWindow->contentItem());

    auto url = transUrl("qrc:/prism_qt_ui/window/QmlDebugWindow.qml");
    component.loadUrl(url);
    if ( component.isReady() )
    {
        auto a =   component.create();
        qmlDebugWindow* win =  reinterpret_cast<qmlDebugWindow*>(a);
        win->setViewModel(QVariant::fromValue(viewModel));

        win->setLoadUrl(qmlUrl);
    }
    else
        qWarning() << component.errorString();

}



}// namespace prism::qt::ui
