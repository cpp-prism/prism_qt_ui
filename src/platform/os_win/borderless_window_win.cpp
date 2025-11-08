#include "src/platform/i_borderless_window_helper.h"
#pragma warning(disable : 4996)
#include "borderless_window_win.h"
#include <QDebug>
#include <QVariant>
#include <QScreen>
#include <Windows.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <windowsx.h>
#include <prism/container.hpp>
#include <QQuickItem>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "uxtheme.lib")

namespace prism::qt::ui {

void borderless_window_win::maximize() {
    HWND hwnd = (HWND)winId();
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
}

void borderless_window_win::minimize() {
    HWND hwnd = (HWND)winId();
    ShowWindow(hwnd, SW_SHOWMINIMIZED);
}

void borderless_window_win::restore() {
    HWND hwnd = (HWND)winId();
    ShowWindow(hwnd, SW_RESTORE);
}


borderless_window_win::borderless_window_win(QQuickWindow* parent)
    : QQuickWindow(parent)
{

    auto windowhelper = prism::Container::get()->resolve_object<i_borderless_window_helper>();
    auto winid = this->winId();
    windowhelper->enableWindowBorderless(winid);

    this->setColor("transparent");

    connect(this, &QQuickWindow::screenChanged, this, [this](QScreen* screen) {
        Q_UNUSED(screen)
        // QTBUG-95925 重新触发尺寸修复
        this->resize(this->size());
    });
}


bool borderless_window_win::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
    if (eventType != "windows_generic_MSG")
        return false;

    MSG* msg = static_cast<MSG*>(message);
    if (!msg)
        return false;

    switch (msg->message)
    {
    case WM_SHOWWINDOW:
    {
        BOOL composition_enabled = FALSE;
        bool success = (::DwmIsCompositionEnabled(&composition_enabled) == S_OK);
        //bool isClassic = !(composition_enabled && success);

        // 恢复原生样式，使其仍可最小化、最大化、拖动
        SetWindowLongPtr(msg->hwnd, GWL_STYLE, WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN);
        return false;
    }

    case WM_NCCALCSIZE:
    {
        if (msg->wParam)
        {
            NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
            if (params->rgrc[0].top != 0)
                params->rgrc[0].top -= 1;
        }
        *result = WVR_REDRAW;
        return true;
    }


    case WM_NCHITTEST:
    {
        const LONG borderWidth = 8;
        RECT winrect;
        GetWindowRect(msg->hwnd, &winrect);

        const LONG x = GET_X_LPARAM(msg->lParam);
        const LONG y = GET_Y_LPARAM(msg->lParam);

        qreal scale = 1.0;
        if (screen())
            scale = screen()->devicePixelRatio();

        // 标题栏区域点击
        if (x >= winrect.left + borderWidth &&
            x < winrect.right - static_cast<LONG>(titleBarBtnWidth() * scale) &&
            y >= winrect.top + borderWidth &&
            y < winrect.top + static_cast<LONG>(titleHeight() * scale))
        {
            *result = HTCAPTION;
            return true;
        }

        // 左下角
        if (x >= winrect.left && x < winrect.left + borderWidth &&
            y < winrect.bottom && y >= winrect.bottom - borderWidth)
        {
            *result = HTBOTTOMLEFT;
            return true;
        }

        // 右下角
        if (x < winrect.right && x >= winrect.right - borderWidth &&
            y < winrect.bottom && y >= winrect.bottom - borderWidth)
        {
            *result = HTBOTTOMRIGHT;
            return true;
        }

        // 左上角
        if (x >= winrect.left && x < winrect.left + borderWidth &&
            y >= winrect.top && y < winrect.top + borderWidth)
        {
            *result = HTTOPLEFT;
            return true;
        }

        // 右上角
        if (x < winrect.right && x >= winrect.right - borderWidth &&
            y >= winrect.top && y < winrect.top + borderWidth)
        {
            *result = HTTOPRIGHT;
            return true;
        }

        // 左边框
        if (x >= winrect.left && x < winrect.left + borderWidth)
        {
            *result = HTLEFT;
            return true;
        }

        // 右边框
        if (x < winrect.right && x >= winrect.right - borderWidth)
        {
            *result = HTRIGHT;
            return true;
        }

        // 下边框
        if (y < winrect.bottom && y >= winrect.bottom - borderWidth)
        {
            *result = HTBOTTOM;
            return true;
        }

        // 上边框
        if (y >= winrect.top && y < winrect.top + borderWidth)
        {
            *result = HTTOP;
            return true;
        }

        return false;
    }

    case WM_ACTIVATE:
    case WM_SETFOCUS:
    {
        // Windows 7 再次触发 Frame 更新
        if (QOperatingSystemVersion::current() <= QOperatingSystemVersion(QOperatingSystemVersion::Windows, 7))
            SetWindowPos(msg->hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        return false;
    }

    case WM_NCACTIVATE:
    {
        if (!::IsIconic(msg->hwnd) && ::IsWindowEnabled(msg->hwnd))
            return true;
        return false;
    }

    default:
        break;
    }

    return false;
}

int borderless_window_win::titleHeight() const
{
    return m_titleHeight;
}

void borderless_window_win::setTitleHeight(int newTitleHeight)
{
    if (m_titleHeight == newTitleHeight)
        return;
    m_titleHeight = newTitleHeight;
    emit titleHeightChanged();
}

int borderless_window_win::titleBarBtnWidth() const
{
    return m_titleBarBtnWidth;
}

void borderless_window_win::setTitleBarBtnWidth(int newTitleBarBtnWidth)
{
    if (m_titleBarBtnWidth == newTitleBarBtnWidth)
        return;
    m_titleBarBtnWidth = newTitleBarBtnWidth;
    emit titleBarBtnWidthChanged();
}

} // namespace prism::qt::ui
