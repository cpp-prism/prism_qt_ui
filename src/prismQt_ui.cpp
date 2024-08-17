#include "prismQt_ui.h"
#include "controls/forms/datepickercpp.h"
#include "controls/opengl/logorenderer.h"
#include "controls/opengl/threadrenderer.h"
#include "controls/shapes/video_roi_cpp.h"
#include "controls/window/qml_debug_window.h"
#include "helper/menuhelper.h"
#include "include/prism/qt/ui/helper/cpp_utility.h"
#include "include/prism/qt/ui/img_buffer_Info.h"
#include "include/prism/qt/ui/model/roi_info.h"
#include "validator/datetime_validator.h"
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformintegration.h>
#include <prism/container.hpp>
#include <qqml.h>

using prism::qt::core::prismModelProxy;
namespace prism::qt::ui
{
static void registerBorderlessHleper();

prismQt_ui::prismQt_ui()
{
}

prismQt_ui::~prismQt_ui()
{
}

bool prismQt_ui::register_types()
{
    //控件注册
    qmlRegisterType<qmlDebugWindow>("prismCpp", 1, 0, "QmlDebugWindowCPP");
    qmlRegisterRevision<QWindow, 1>("prismCpp", 1, 0);
    qmlRegisterRevision<QQuickWindow, 1>("prismCpp", 1, 0);

    //注册多平台无边框窗口
    registerBorderlessHleper();

    // opengl渲染器
    qmlRegisterType<ThreadRenderer>("prism_qt_ui", 1, 0, "Renderer");
    qmlRegisterType<datetime_validator>("prism_qt_ui", 1, 0, "DatetimeValidator");
    // datetimepicker
    qmlRegisterType<DatePickerCpp>("prism_qt_ui_private", 1, 0, "DatePickerCpp");
    qmlRegisterType<video_roi_cpp>("prism_qt_ui_private", 1, 0, "Video_roi_cpp");
    //注册相机帧信息
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<prism::qt::ui::img_frame_info>();
    qRegisterMetaType<prismModelProxy<prism::qt::ui::img_frame_info>*>("prismModelProxy<prism::qt::ui::img_frame_info>*");
    qRegisterMetaType<std::shared_ptr<bool>>("std::shared_ptr<bool>");

    //注册qml中使用的实用工具类到ioc container 和qml engine
    std::shared_ptr<cpp_utility> sp_cpputility = std::make_shared<cpp_utility>(new cpp_utility());
    prism::Container::get()->register_instance<cpp_utility>(sp_cpputility);

    //注册单例 到qml engine  qt 5.15的语法
    // qmlRegisterSingletonInstance<cpp_utility>("prismCpp", 1, 0, "CppUtility",sp_cpputility.get());
    //注册单例 到qml engine  qt 5.12的语法
    qmlRegisterSingletonType<cpp_utility>("prismCpp", 1, 0, "CppUtility", [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return prism::Container::get()->resolve_object<cpp_utility>().get();
    });

    //注册单例 到qml engine  qt 5.15的语法
    // qmlRegisterSingletonInstance<MenuHelper>("prismCpp", 1, 0, "MenuHelper", new MenuHelper());
    //注册单例 到qml engine  qt 5.12的语法
    qmlRegisterSingletonType<MenuHelper>("prismCpp", 1, 0, "MenuHelper", [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new MenuHelper();
    });

    return true;
}

bool prismQt_ui::init()
{

    std::shared_ptr<QQmlApplicationEngine> engine = Container::get()->resolve_object<QQmlApplicationEngine>();
    cpp_utility a;
    if (!a.enableHotReload())
        engine->addImportPath("qrc:/");
    else
    {
        engine->addImportPath(QString("%1/%2").arg(PRISMQT_SOLUTION_SOURCEDIR).arg("prism_qt_ui/src"));
    }
    return true;
}

bool prismQt_ui::install()
{
// macos 使用metal
#if _WIN32 || __linux
    if (!QGuiApplicationPrivate::platform_integration->hasCapability(QPlatformIntegration::ThreadedOpenGL))
    {
        qDebug() << "QPlatformIntegration::ThreadedOpenGL 兼容检测不通过";
    }
    QOpenGLContext context;
    context.create();
    if (context.isValid())
    {
        qDebug() << "OpenGL Version: " << context.format().majorVersion() << "." << context.format().minorVersion();
    }
    else
    {
        qDebug() << "OpenGL is not available.";
    }
#endif

    return true;
}

bool prismQt_ui::uninstall()
{

    return true;
}

bool prismQt_ui::uninit()
{

    return true;
}

} // namespace prism::qt::ui

#ifdef Q_OS_WIN
#include "platform/os_win/borderless_window_helpe_win.h"
#include "platform/os_win/borderless_window_win.h"
#include "platform/os_win/multi_screen_helper_win.h"
#endif

#ifdef Q_OS_MACX
#include "src/platform/os_macx/borderless_window_mac.h"
#endif

namespace prism::qt::ui
{

#ifdef Q_OS_WIN
static void registerBorderlessHleper()
{
    // dpi evn
    // auto scinfo = multi_screen_helper_win ::getPrimaryScreenInfo();
    // qputenv("QT_SCALE_FACTOR", QString("%1").arg(scinfo.scalingFactor).toLatin1());

    // borderlesswindowhelper
    using IborderHelper = i_borderless_window_helper;
    using borderHelper = borderless_window_helpe_win;
    Container::get()->register_instance<IborderHelper>(std::make_shared<borderHelper>());

    //注册window
    qmlRegisterType<borderless_window_win>("prism_qt_ui", 1, 0, "BorderLessWindow_Win_cpp");
    qmlRegisterRevision<QWindow, 1>("prism_qt_ui", 1, 0);
    qmlRegisterRevision<QQuickWindow, 1>("prism_qt_ui", 1, 0);
}
#endif

#ifdef Q_OS_MACX

#include "src/platform/os_macx/borderless_window_mac.h"

static void registerBorderlessHleper()
{
    using IborderHelper = i_borderless_window_helper;
    using borderHelper = borderless_window_mac;
    prism::Container::get()->register_instance<IborderHelper>(std::make_shared<borderHelper>());
}
#endif

#ifdef Q_OS_LINUX
static void registerBorderlessHleper()
{
}
#endif

} // namespace prism::qt::ui
