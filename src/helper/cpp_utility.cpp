#include "include/prism/qt/ui/helper/cpp_utility.h"
#include "../platform/i_borderless_window_helper.h"
#include "src/controls/window/qml_debug_window.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFileInfo>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>
#include <prism/container.hpp>

#include <QDebug>

#ifdef USING_PCL
#include <QQuickVTKRenderWindow.h>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QSurfaceFormat>
#endif

namespace prism::qt::ui
{
cpp_utility::cpp_utility(QObject* parent)
    : QObject(parent)
{
    // detect ui hang timer
    uiTimer_ = std::make_unique<QTimer>();
    setCppOwnership(uiTimer_.get());
    connect(uiTimer_.get(), &QTimer::timeout, this, [this]() {
        pre_timepoint_ = QDateTime::currentMSecsSinceEpoch();
    });

    // detect qml live
    int length = QCoreApplication::arguments().length();
    for (int i = 0; i < length; ++i)
    {
        QString arg = QCoreApplication::arguments().at(i);

        if (arg == "qml_live")
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
    if (uiTimer_)
        uiTimer_->stop();
}

bool cpp_utility::is_ui_hang()
{
    auto delta = QDateTime::currentMSecsSinceEpoch() - pre_timepoint_;
    if (delta > uiTimer_interval_ * 10)
        return true;
    else
        return false;
}

void cpp_utility::setQmlOwnership(QObject* obj)
{
    if (QQmlEngine::objectOwnership(obj) != QQmlEngine::JavaScriptOwnership)
    {
        QQmlEngine::setObjectOwnership(obj, QQmlEngine::ObjectOwnership::JavaScriptOwnership);
    }
}

void cpp_utility::setCppOwnership(QObject* obj)
{
    if (QQmlEngine::objectOwnership(obj) != QQmlEngine::CppOwnership)
    {
        QQmlEngine::setObjectOwnership(obj, QQmlEngine::ObjectOwnership::CppOwnership);
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
        QString arg;
        for (int i = 0; i < length; ++i)
        {
            arg = QCoreApplication::arguments().at(i);
            if (arg.startsWith("qml_live"))
            {
                qml_live_flag = 1;
                return qml_live_flag;
            }
        }
        qml_live_flag = 0;
    }
    return qml_live_flag;
}

std::optional<bool> cpp_utility::bool2opt(bool b)
{
    return std::make_optional<bool>(b);
}

QUrl cpp_utility::transUrl(QString url)
{
    if (enableHotReload())
    {
        static int prismSourceLevel = -1;
        if (prismSourceLevel == -1)
        {
            int length = QCoreApplication::arguments().length();
            QString arg;
            bool hit = false;
            for (int i = 0; i < length; ++i)
            {
                arg = QCoreApplication::arguments().at(i);
                if (arg.startsWith("prismSourceLevel"))
                {
                    QStringList items = arg.split(':');
                    prismSourceLevel = items.at(1).toUInt();
                    hit = true;
                    break;
                }
            }
            if (!hit)
                prismSourceLevel = 0;
        }

        if (url.isEmpty())
            return QUrl();
        QString root(PRISMQT_SOLUTION_SOURCEDIR);
        QString relativePath;
        if (url.startsWith("qrc:/prism_qt_ui"))
            relativePath = "";
        else
            for (int i = 0; i < prismSourceLevel + 1; ++i)
            {
                relativePath += "../";
            }
        auto rx = QRegExp(QString::fromStdString(R"(^qrc:/([\w-_]+)(.*))"));
        rx.lastIndexIn(url);
        QString url_replaced = QString("file:///%1/%2%3/src/%4%5").arg(root).arg(relativePath).arg(rx.cap(1)).arg(rx.cap(1)).arg(rx.cap(2));
        // qDebug()<< "trans url :" << url_replaced;
        return QUrl(url_replaced);
    }
    return url;
}

QString cpp_utility::qurl2localfile(QUrl url)
{
    return url.toLocalFile();
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
    return QCursor::pos();
}

void cpp_utility::forceUpdateMouseAreaHovered(QObject* mouseArea)
{
    // QEvent* e = new QEvent(QEvent::MouseMove);
    // QCoreApplication::sendEvent(mouseArea, e);

    //{
    //    QMouseEvent *enterEvent = new QMouseEvent(QEvent::Enter, QPointF(10, 10), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    //    QCoreApplication::sendEvent(mouseArea, enterEvent);
    //}

    //{
    //    QMouseEvent *exitEvent = new QMouseEvent(QEvent::Leave, QPointF(200, 200), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    //    QCoreApplication::sendEvent(mouseArea, exitEvent);
    //}

    {
        QMouseEvent* exitEvent = new QMouseEvent(QEvent::MouseMove, QPointF(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        QCoreApplication::sendEvent(mouseArea, exitEvent);
    }
}

QModelIndex cpp_utility::getInvalidQModelIndex()
{
    return QModelIndex();
}

std::shared_ptr<bool> cpp_utility::showQmlDialog(QString qmlUrl, QObject* viewModel)
{
    std::shared_ptr<bool> result;
    std::shared_ptr<QQmlApplicationEngine> p_engine = Container::get()->resolve_object<QQmlApplicationEngine>();
    QQmlComponent component(p_engine.get());

    QQuickWindow::setDefaultAlphaBuffer(true);
    auto url = transUrl("qrc:/prism_qt_ui/window/QmlDebugWindow.qml");
    component.loadUrl(url);
    qmlDebugWindow* win = nullptr;
    if (component.isReady())
    {
        QEventLoop loop;
        auto a = component.create();
        win = reinterpret_cast<qmlDebugWindow*>(a);
        win->setViewModel(QVariant::fromValue(viewModel));
        QObject::connect(
            win, &QQuickWindow::destroyed, &loop, [&](auto* p) {
                Q_UNUSED(p)
                loop.exit();
            },
            Qt::ConnectionType::QueuedConnection);

        win->setLoadUrl(qmlUrl);
        loop.exec();

        //#undef Bool
        // if(win->dialogResult().canConvert(QVariant::Bool))
        //{
        //    result = std::make_shared<bool>(win->dialogResult().value<bool>());
        //}

        // loop.disconnect(win);

        // if(win)
        //     win->deleteLater();
    }
    else
    {
        qWarning() << component.errorString();
        throw component.errorString().toStdString().c_str();
    }

    return result;
}

void cpp_utility::showQmlWindow(QString qmlUrl, QObject* viewModel)
{
    QQuickWindow::setDefaultAlphaBuffer(true);

    std::shared_ptr<QQmlApplicationEngine> p_engine = Container::get()->resolve_object<QQmlApplicationEngine>();
    std::shared_ptr<QQuickWindow> mainWindow = prism::Container::get()->resolve_object<QQuickWindow>();
    QQmlComponent component(p_engine.get(), mainWindow->contentItem());

    auto url = transUrl("qrc:/prism_qt_ui/window/QmlDebugWindow.qml");
    component.loadUrl(url);
    if (component.isReady())
    {
        auto a = component.create();
        qmlDebugWindow* win = reinterpret_cast<qmlDebugWindow*>(a);
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
        std::shared_ptr<QTranslator> translator = prism::Container::get()->resolve_object<QTranslator>();
        if (!translator)
        {
            if (!QCoreApplication::removeTranslator(translator.get()))
            {
                std::shared_ptr<QTranslator> empty;
                prism::Container::get()->register_instance(empty);
            }
            else
                throw std::string("卸载多语言翻译器失败");
        }

        if (!filePath.isEmpty())
        {
            std::shared_ptr<QTranslator> tr = std::make_shared<QTranslator>();
            if (tr->load(filePath))
            {
                if (QCoreApplication::installTranslator(tr.get()))
                    prism::Container::get()->register_instance(tr);
                else
                    throw std::string("安装多语言翻译器失败");
            }
            else
                throw std::string("加载多语言文件失败:") + filePath.toStdString();
        }
        else
        {
            qInfo() << "正在卸载非中文的多语言翻译器";
        }
        //通知qml engine，如果有的话
        std::shared_ptr<QQmlApplicationEngine> engine = Container::get()->resolve_object<QQmlApplicationEngine>();
        if (engine)
        {
            /**
             * fix 1 - retranslate will refresh all qml property,cause qsortfilterproxymodel* get an freed address which is incorret
             * fix 2 - Performance when switching languages ​​dynamically
             * by hbb 2024.2.3
             */
            // engine->retranslate();
            if (transThis() == "")
                setTransThis("\u200B"); // ZWSP
            else
                setTransThis("");
        }
    }
    catch (std::string& mes)
    {
        qCritical() << QString::fromStdString(mes);
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

void cpp_utility::openPath(const QString path)
{
    QFileInfo fileInfo(path);

    if (fileInfo.isDir())
    {
        // 如果是目录，直接打开目录
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
    else if (fileInfo.isFile())
    {
        // 如果是文件，打开文件所在的目录，并定位到文件
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
    }
    else
    {
        // 路径既不是目录也不是文件，显示错误信息
        qDebug() << "open invalidate path :" << path;
    }
}

QObject* cpp_utility::getModelIndexInternalPointer(const QModelIndex& item)
{
    return static_cast<QObject*>(item.internalPointer());
}

QString cpp_utility::getguid()
{
    QUuid uuid = QUuid::createUuid();
    return uuid.toString();
}

bool cpp_utility::isqmllive()
{
    return isqmllive_;
}

std::string cpp_utility::openGLVersion = "default";

const QString& cpp_utility::transThis() const
{
    return m_transThis;
}

void cpp_utility::setTransThis(const QString& newTransThis)
{
    if (m_transThis == newTransThis)
        return;
    m_transThis = newTransThis;
    emit transThisChanged();
}

QColorDialog* cpp_utility::openColorWindow(QWidget* parent)
{
    QColorDialog* d = new QColorDialog(parent);

    d->setWindowTitle("选择颜色");
    d->setWindowFlags(d->windowFlags() | Qt::WindowStaysOnTopHint);
    d->exec();
    return d;
}

int cpp_utility::getDialogResult(QDialog* d)
{
    return d->result();
}

QString cpp_utility::qcolor2qstring(QColor color)
{
    return color.name();
}

} // namespace prism::qt::ui

#ifdef USING_PCL
void initVTK()
{
    qDebug() << "set openGL version " << QString::fromStdString(prism::qt::ui::cpp_utility::openGLVersion);
    if (prism::qt::ui::cpp_utility::openGLVersion == "3.2")
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
    else if (prism::qt::ui::cpp_utility::openGLVersion == "3.0")
    {
        std::string mesa_gl_version = std::string("MESA_GL_VERSION_OVERRIDE=") + prism::qt::ui::cpp_utility::openGLVersion;

        qDebug() << QString::fromStdString(mesa_gl_version);

        char* env = (char*)mesa_gl_version.c_str();
#ifdef __linux
#define DV_PUTENV putenv
#else
#define DV_PUTENV _putenv
#endif
        DV_PUTENV(env);

        DV_PUTENV((char*)"MESA_GL_VERSION_OVERRIDE=3.0");
        // Fixes decimal point issue in vtkSTLReader
        DV_PUTENV((char*)"LC_NUMERIC=C");
    }
}
#endif
