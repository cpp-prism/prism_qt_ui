#ifndef PRISM_QT_UI_BORDERLESS_WINDOW_HELPE_WIN_H
#define PRISM_QT_UI_BORDERLESS_WINDOW_HELPE_WIN_H

#include "../i_borderless_window_helper.h"
#include <QAbstractNativeEventFilter>
#include <QMap>
#include <QWindow>
#include <Windows.h>

namespace prism::qt::ui {
// we cannot just use WS_POPUP style
// WS_THICKFRAME: without this the window cannot be resized and so aero snap, de-maximizing and minimizing won't work
// WS_SYSMENU: enables the context menu with the move, close, maximize, minize... commands (shift + right-click on the
// task bar item) WS_CAPTION: enables aero minimize animation/transition WS_MAXIMIZEBOX, WS_MINIMIZEBOX: enable
// minimize/maximize
enum class style : DWORD
{
    aero_borderless = (WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN),
    windowed = (WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN),
};

class borderless_window_helpe_win : public i_borderless_window_helper
{
  public:
    borderless_window_helpe_win();

    // IWindowBorderlessHelper interface
  public:
    void enableWindowBorderless(qulonglong wId) override;
};

}// namespace prism::qt::ui

#endif // PRISM_QT_UI_BORDERLESS_WINDOW_HELPE_WIN_H
