#include "borderless_window_helpe_win.h"
#include <QDebug>
#include <QSysInfo>
#include <QVector>
#include <dwmapi.h>
#pragma warning(disable: 4996)

namespace prism::qt::ui {
borderless_window_helpe_win::borderless_window_helpe_win()
{
}

void borderless_window_helpe_win::enableWindowBorderless(qulonglong wId)
{
    bool iswin7classic = false;
    BOOL composition_enabled = FALSE;
    bool success = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
    iswin7classic = !(composition_enabled && success);

    HWND wnd = (HWND)wId;
    if (iswin7classic && QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7)
    {
        SetWindowLongPtr(wnd, GWL_STYLE, static_cast<LONG>(style::aero_borderless));
    }
    else
    {
        SetWindowLongPtr(wnd, GWL_STYLE, static_cast<LONG>(style::aero_borderless));
        // enable shadow
        const MARGINS shadow_on = {1, 1, 1, 1};
        DwmExtendFrameIntoClientArea(wnd, &shadow_on);
    }

    //redraw frame
    SetWindowPos(wnd, Q_NULLPTR, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(wnd, SW_SHOW);
}
}// namespace prism::qt::ui
