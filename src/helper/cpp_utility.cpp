#include "include/prism/qt/ui/helper/cpp_utility.h"
#include "../platform/i_borderless_window_helper.h"
#include "src/controls/window/qml_debug_window.h"
#include <QDir>
#include <QFileInfoList>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFileInfo>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>
#include <prism/container.hpp>

#include <QDebug>
#include <QRegularExpression>

#ifdef PRISMUI_PCL
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

QString cpp_utility::translate(QString source)
{
   QString r =  _trans_dic[source] ;
   if(r.isEmpty() || r.isNull())
       return source;
   else
       return r;
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QRegExp rx(QString::fromStdString(R"(^qrc:/([\w-_]+)(.*))"));
        rx.lastIndexIn(url);
        QString url_replaced = QString("file:///%1/%2%3/src/%4%5")
                                   .arg(root)
                                   .arg(relativePath)
                                   .arg(rx.cap(1))
                                   .arg(rx.cap(1))
                                   .arg(rx.cap(2));
#else
        static QRegularExpression rx(QString::fromStdString(R"(^qrc:/([\w_-]+)(.*))"));
        QRegularExpressionMatch match = rx.match(url);
        QString url_replaced;
        if (match.hasMatch()) {
            url_replaced = QString("file:///%1/%2%3/src/%4%5")
                               .arg(root)
                               .arg(relativePath)
                               .arg(match.captured(1))
                               .arg(match.captured(1))
                               .arg(match.captured(2));
        } else {
            url_replaced = url;
        }

#endif

        // qDebug()<< "trans url :" << url_replaced;
        return QUrl(url_replaced);
    }
    return url;
}

QString cpp_utility::qurl2localfile(QUrl url)
{
    return url.toLocalFile();
}

QUrl cpp_utility::localfile2qurl(QString path)
{
    return QUrl::fromLocalFile(path);
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

void cpp_utility::s_centerQmlWindow2cursorScreen(QQuickWindow *win)
{
    const QPoint cursorPos = QCursor::pos();
    QScreen *screen = QGuiApplication::screenAt(cursorPos);

    if (!screen)
        screen = QGuiApplication::primaryScreen();

    const QRect avail = screen->availableGeometry();
    const QSize size = win->size();

    const QPoint topLeft(
        avail.x() + (avail.width() - size.width()) / 2,
        avail.y() + (avail.height() - size.height()) / 2
        );

    win->setPosition(topLeft);
}

void cpp_utility::centerQmlWindow2cursorScreen(QQuickWindow *win)
{
    s_centerQmlWindow2cursorScreen(win);
}

void cpp_utility::showQmlWindow(QString winUrl, QObject* viewModel)
{
    QQuickWindow::setDefaultAlphaBuffer(true);

    std::shared_ptr<QQmlApplicationEngine> p_engine = Container::get()->resolve_object<QQmlApplicationEngine>();
    std::shared_ptr<QQuickWindow> mainWindow = prism::Container::get()->resolve_object<QQuickWindow>();
    QQmlComponent component(p_engine.get(), mainWindow->contentItem());

    auto url = transUrl(winUrl);
    component.loadUrl(url);
    if (component.isReady())
    {
        auto a = component.create();
        qmlDebugWindow* win = reinterpret_cast<qmlDebugWindow*>(a);
        win->setViewModel(QVariant::fromValue(viewModel));
        win->setPersistentGraphics(true);

    }
    else
        qWarning() << component.errorString();
}
//void cpp_utility::showQmlWindow(QString winurl, QObject* viewModel)
//{
//    QQuickWindow::setDefaultAlphaBuffer(true);
//
//    std::shared_ptr<QQmlApplicationEngine> p_engine = Container::get()->resolve_object<QQmlApplicationEngine>();
//    std::shared_ptr<QGuiApplication> app = prism::Container::get()->resolve_object<QGuiApplication>();
//
//    static QMetaObject::Connection connection = QObject::connect(
//        p_engine.get(),
//        &QQmlApplicationEngine::objectCreated,
//        app.get(),
//        [&](QObject* object, const QUrl& url) {
//            if (url.toString() == winurl)
//            {
//                if (!object)
//                {
//                    app->exit(-1);
//                }
//
//                auto* win = reinterpret_cast<qmlDebugWindow*>(object);
//                if (win)
//                {
//                    QObject::disconnect(connection);
//
//                    std::shared_ptr<QQuickWindow> sp_win(win, [](QQuickWindow* p) { Q_UNUSED(p) });
//                    prism::Container::get()->register_instance(sp_win);
//
//                    win->setViewModel(QVariant::fromValue(viewModel));
//                }
//            }
//        },
//        Qt::QueuedConnection);
//        p_engine->load(winurl);
//}

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
            QQmlComponent transComp (engine.get(),QUrl(TransFile));
            QObject* listmodel = transComp.create();
            if(listmodel)
            {
                QAbstractItemModel* itemmodel = qobject_cast<QAbstractItemModel*>(listmodel);
                if(itemmodel)
                {
                    for(int i = 0; i< itemmodel->rowCount(); ++i)
                    {
                        QModelIndex idx = itemmodel->index(i,0);

                        QString source = itemmodel->data(idx,1).value<QString>();
                        QString translate = itemmodel->data(idx,0).value<QString>();
                        //qDebug()<< "source:" << source << "    tanslate:" << translate;
                        _trans_dic[source] = translate;
                    }
                }
            }

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

QString cpp_utility::openPath(const QString path)
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
        return path;
    }
    return "";
}

QString cpp_utility::removeDirRecursive(QString path)
{
    QDir rmd (path);
    if(!rmd.exists())
        return "";
    QFileInfoList infolist =  rmd.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries);

    for(const QFileInfo& info : infolist)
    {
        if(info.isDir())
        {
            QString result =  removeDirRecursive(info.absoluteFilePath());
            if(!(result.isEmpty() || result.isNull()))
                return result;
        }
        else
        {
            if(!QFile::remove(info.absoluteFilePath()))
            {
                return QString("failed to remove file : %1 ").arg(info.absoluteFilePath());
            }
        }
    }

    if(!rmd.rmdir(path))
    {
        return QString("failed to remove dir : %1 ").arg(path);
    }

    return "";

}

QObject* cpp_utility::getModelIndexInternalPointer(const QModelIndex& item)
{
    return reinterpret_cast<QObject*>(item.internalPointer());
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

QString cpp_utility::getAppBaseDir()
{
    QString dir = QCoreApplication::instance()->applicationDirPath();
    return dir;
}

QString cpp_utility::base64Encode(const QString &input)
{
    return input.toUtf8().toBase64();
}

QString cpp_utility::base64Decode(const QString &base64)
{
    QByteArray decoded = QByteArray::fromBase64(base64.toUtf8());
    return QString::fromUtf8(decoded);
}

bool cpp_utility::killProceById(int pid)
{
#if defined(Q_OS_WIN)
    // Windows 上使用 `taskkill` 命令
    QString command = "taskkill /PID " + QString::number(pid) + " /F";
#elif defined(Q_OS_UNIX)
    // Unix 系统使用 `kill` 命令
    QString command = "kill -9 " + QString::number(pid);
#else
    qWarning() << "Unsupported platform!";
    return false;
#endif

    // 使用 QProcess 执行命令
    QProcess process;
    process.start(command);
    process.waitForFinished();

    if (process.exitCode() == 0)
    {
        qDebug() << "Process" << pid << "terminated successfully!";
        return true;
    }
    else
    {
        qDebug() << "Failed to terminate process" << pid << "!";
        return false;
    }
}

bool cpp_utility::isForegroundShell(int pid)
{
#if defined(Q_OS_WIN)
    return false;
    QString command = "taskkill /PID " + QString::number(pid) + " /F"; // TODO windows 待实现
                                                                       // tasklist /v /FI "PID eq 12345"  //通过此法貌,过滤窗口标题的方式貌似可以查看是否是在与用户交互,待测试
#elif defined(Q_OS_UNIX)
    QString command = QString("bash -c \"ps u -p %1 | awk '{print $8}' \" ").arg(QString::number(pid));
    // 使用 QProcess 执行命令
    QProcess process;
    process.start(command);
    process.waitForFinished();

    if (process.exitCode() == 0)
    {
        QByteArray output = process.readAllStandardOutput();
        if (output.contains("Ss+"))
            return true;
        else
            return false;
    }
    else
    {
        qDebug() << "Failed to exec  :" << command << "!  :" << process.errorString();
        return false;
    }
#else
    return false;

#endif
}

QQmlEngine* cpp_utility::getqmlEngine()
{
    std::shared_ptr<QQmlApplicationEngine> engine = Container::get()->resolve_object<QQmlApplicationEngine>();
    return engine.get();
}

QJSValue cpp_utility::qsTr(const QString source)
{
    std::shared_ptr<QQmlApplicationEngine> engine = Container::get()->resolve_object<QQmlApplicationEngine>();
    QString placehold = " cpputility.transThis + cpputility.translate(\"$source$\")";
    QJSValue result ;
    QJSValue a = engine->newQObject(this);
    engine->globalObject().setProperty("cpputility",a);
    if(engine)
    {
        try{
            result = engine->evaluate(placehold.replace("$source$",source));
        }
        catch(...)
        {

        }
    }

    if (result.isError()) {
        qWarning() << "Error evaluating JavaScript expression:" << result.toString();
        //return QJSValue();
    }

    return result;
}

void cpp_utility::s_createFileAndWrite(const QString &path, const QString &content)
{
    QFileInfo fileInfo(path);
    QDir dir(fileInfo.dir().dirName());
    if (!dir.exists())
        dir.mkpath(fileInfo.dir().dirName());

    QFile file(path);

    // 以写入模式打开文件，如果文件不存在则创建，已存在则覆盖
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif
        out << content;
        file.close();
    } else {
        qWarning() << "无法打开文件:" << file.errorString();
    }
}

void cpp_utility::createFileAndWrite(const QString& path,const QString& content)
{
    return s_createFileAndWrite(path,content);
}

} // namespace prism::qt::ui

#ifdef PRISMUI_PCL
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
