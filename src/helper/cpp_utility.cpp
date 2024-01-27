#include "include/prism/qt/ui/helper/cpp_utility.h"
#include "src/controls/window/qml_debug_window.h"
#include "../platform/i_borderless_window_helper.h"
#include <prism/container.hpp>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>

#include <QDebug>

#ifdef USING_PCL
    #include <QQuickVTKRenderWindow.h>
    #include <QQuickWindow>
    #include <QSGRendererInterface>
    #include <QSurfaceFormat>
#endif

namespace prism::qt::ui {
cpp_utility::cpp_utility(QObject* parent)
    : QObject(parent)
{
    //detect ui hang timer
    uiTimer_ = std::make_unique<QTimer>();
    connect(uiTimer_.get(),&QTimer::timeout,this,[this](){
        pre_timepoint_ = QDateTime::currentMSecsSinceEpoch();
    });


    //detect qml live
    int length = QCoreApplication::arguments().length();
    for(int i = 0; i < length; ++i)
    {
        QString arg = QCoreApplication::arguments().at(i);

        if(arg == "qml_live")
        {
            isqmllive_ = true;
        }
    }


    uiTimer_->setSingleShot(false);
    uiTimer_->setInterval(uiTimer_interval_);
    uiTimer_->start();

}

cpp_utility::~cpp_utility()
{
    if(uiTimer_)
        uiTimer_->stop();
}

bool cpp_utility::is_ui_hang()
{
    auto delta = QDateTime::currentMSecsSinceEpoch() - pre_timepoint_;
    if(delta > uiTimer_interval_*10)
        return true;
    else
        return false;
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

QModelIndex cpp_utility::getInvalidQModelIndex()
{
    return QModelIndex();
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

bool cpp_utility::load_language_qm(QString filePath)
{
    try
    {
        std::shared_ptr<QTranslator> translator =  prism::Container::get()->resolve_object<QTranslator>();
        if(!translator)
        {
            if(!QCoreApplication::removeTranslator(translator.get()))
            {
                std::shared_ptr<QTranslator> empty ;
                prism::Container::get()->register_instance(empty);
            }
            else
                throw std::string("卸载多语言翻译器失败");
        }

        if(!filePath.isEmpty())
        {
            std::shared_ptr<QTranslator> tr = std::make_shared<QTranslator>();
            if(tr->load(filePath))
            {
                if(QCoreApplication::installTranslator(tr.get()))
                    prism::Container::get()->register_instance(tr);
                else
                    throw std::string("安装多语言翻译器失败");

            }
            else
                throw std::string("加载多语言文件失败:") + filePath.toStdString();

        }
        else
        {
            qInfo()<<"正在卸载非中文的多语言翻译器";
        }
        //通知qml engine，如果有的话
        std::shared_ptr<QQmlApplicationEngine> engine = Container::get()->resolve_object<QQmlApplicationEngine>();
        if(engine)
            engine->retranslate();
    }
    catch(std::string & mes)
    {
        qCritical()<< QString::fromStdString(mes);
        return false;
    }
    return true;
}

void cpp_utility::setCursor(Qt::CursorShape cs)
{
    QApplication* app = qobject_cast<QApplication*>(QApplication::instance());
    app->setOverrideCursor(QCursor(cs));
}
void cpp_utility::restoreCursor()
{
    QApplication* app = qobject_cast<QApplication*>(QApplication::instance());
    app->restoreOverrideCursor();
}

bool cpp_utility::isqmllive()
{
        return isqmllive_;
}


std::string cpp_utility::openGLVersion  = "default";
}// namespace prism::qt::ui

#ifdef USING_PCL
void initVTK()
{
    qDebug() << "set openGL version " << QString::fromStdString(prism::qt::ui::cpp_utility::openGLVersion);
    if(prism::qt::ui::cpp_utility::openGLVersion =="3.2")
    {
        QSurfaceFormat fmt;
        fmt.setRenderableType(QSurfaceFormat::OpenGL);
        fmt.setVersion(3, 2);
        fmt.setProfile(QSurfaceFormat::CoreProfile);
        fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        fmt.setRedBufferSize(8);
        fmt.setGreenBufferSize(8);
        fmt.setBlueBufferSize(8);
        fmt.setDepthBufferSize(8);
        fmt.setAlphaBufferSize(8);
        fmt.setStencilBufferSize(0);
        fmt.setStereo(false);
        fmt.setSamples(0); // we never need multisampling in the context since the FBO can support
        // multisamples independently

        QSurfaceFormat::setDefaultFormat(fmt);

        //#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        //    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);
        //#endif
    }
    else if(prism::qt::ui::cpp_utility::openGLVersion == "3.0")
    {
        std::string mesa_gl_version = std::string("MESA_GL_VERSION_OVERRIDE=") + prism::qt::ui::cpp_utility::openGLVersion;

        qDebug()<< QString::fromStdString(mesa_gl_version);

        char* env = (char*)mesa_gl_version.c_str();
        putenv(env);

        putenv((char* )"MESA_GL_VERSION_OVERRIDE=3.0");
        // Fixes decimal point issue in vtkSTLReader
        putenv((char *)"LC_NUMERIC=C");

    }

}
#endif
