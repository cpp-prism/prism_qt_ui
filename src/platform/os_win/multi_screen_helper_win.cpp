#include "multi_screen_helper_win.h"
#include <QDebug>

namespace prism::qt::ui {
static QVector<multi_screen_helper_win::screen_info> m_allScreenInfo;

multi_screen_helper_win::multi_screen_helper_win()
{
}

multi_screen_helper_win::~multi_screen_helper_win()
{
}

QVector<multi_screen_helper_win::screen_info> multi_screen_helper_win::getAllScreensInfo()
{
    updateAllScreenInfo();
    return m_allScreenInfo;
}

multi_screen_helper_win::screen_info multi_screen_helper_win::getPrimaryScreenInfo()
{
    updateAllScreenInfo();

    for (screen_info screen : m_allScreenInfo)
    {
        if (screen.isPrimaryScreen)
        {
            return screen;
        }
    }

    return screen_info();
}

void multi_screen_helper_win::printAllScreenInfo()
{
    updateAllScreenInfo();
    qDebug() << __FUNCTION__ << ":: ################# start to print info of all screens. #################";
    for (screen_info sc : m_allScreenInfo)
    {
        qDebug() << __FUNCTION__ << ":: device name:" << sc.deviceName;
        qDebug() << __FUNCTION__ << ":: friendly name:" << sc.firendlyName;
        qDebug() << __FUNCTION__ << ":: is primary screen:" << sc.isPrimaryScreen;
        qDebug() << __FUNCTION__ << ":: bits per pixel:" << sc.bitsPerPel;
        qDebug() << __FUNCTION__ << ":: physical geometry:" << sc.physicalPosX << sc.physicalPosY << sc.physicalWidth << sc.physicalHeight;
        qDebug() << __FUNCTION__ << ":: system geometry:" << sc.sysPosX << sc.sysPosY << sc.sysWidth << sc.sysHeight;
        qDebug() << __FUNCTION__ << ":: scaling factor:" << sc.scalingFactor;
    }
    qDebug() << __FUNCTION__ << ":: ################# end printing info of all screens. #################";
}

BOOL multi_screen_helper_win::updateAllScreenInfo()
{
    m_allScreenInfo.clear();
    EnumDisplayMonitors(NULL, NULL, multi_screen_helper_win::callbackGetScreenInfo, (LPARAM)&m_allScreenInfo);
    return TRUE;
}

BOOL multi_screen_helper_win::callbackGetScreenInfo(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
{
    MONITORINFOEX mi;
    mi.cbSize = sizeof(mi);
    screen_info sc_info;
    QVector<screen_info>* vpAllScInfo = (QVector<screen_info>*)lParam;
    if (vpAllScInfo == Q_NULLPTR)
    {
        return FALSE;
    }

    ::GetMonitorInfo(hMonitor, &mi);

    DEVMODE dev_mode;
    DISPLAY_DEVICE dev_display;
    ZeroMemory(&dev_mode, sizeof(dev_mode));
    dev_mode.dmSize = sizeof(DEVMODE);

    ZeroMemory(&dev_display, sizeof(dev_display));
    dev_display.cb = sizeof(DISPLAY_DEVICE);

    if (!EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &dev_mode))
    {
        return FALSE;
    }

    if (EnumDisplayDevices(mi.szDevice, 0, &dev_display, 0))
    {
        sc_info.firendlyName = QString::fromLocal8Bit(dev_display.DeviceString);
    }

    sc_info.deviceName = QString::fromLocal8Bit(mi.szDevice);
    sc_info.physicalPosX = dev_mode.dmPosition.x;
    sc_info.physicalPosY = dev_mode.dmPosition.y;
    sc_info.physicalWidth = dev_mode.dmPelsWidth;
    sc_info.physicalHeight = dev_mode.dmPelsHeight;
    sc_info.logicalPixels = dev_mode.dmLogPixels;
    sc_info.scalingFactor = (qreal)dev_mode.dmLogPixels / (qreal)96;
    sc_info.bitsPerPel = dev_mode.dmBitsPerPel;
    sc_info.sysPosX = mi.rcMonitor.left;
    sc_info.sysPosY = mi.rcMonitor.top;
    sc_info.sysWidth = mi.rcMonitor.right - mi.rcMonitor.left;
    sc_info.sysHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
    sc_info.isPrimaryScreen = (mi.dwFlags == MONITORINFOF_PRIMARY);
    sc_info.originInfo = dev_mode;
    vpAllScInfo->push_back(sc_info);
    return TRUE;
}

}// namespace prism::qt::ui
