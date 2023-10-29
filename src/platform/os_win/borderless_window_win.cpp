#pragma warning(disable: 4996)
#include "borderless_window_win.h"
#include <QByteArray>
#include <QDebug>
#include <QVariant>
#include <QWindow>
#include <Windows.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <windowsx.h>
#include <QScreen>

namespace prism::qt::ui {

borderless_window_win::borderless_window_win(QQuickWindow* parent)
    : QQuickWindow(parent)
{
    this->setOpacity(0.0);
    this->setVisible(false);
    this->setHeight(0);
    this->setWidth(0);
    this->setX(-100);
    this->setY(-100);
}

borderless_window_win::borderless_window_win()
{
    connect(this,&QQuickWindow::screenChanged , this,[&]([[maybe_unused]]QScreen *screen){
        //https://bugreports.qt.io/browse/QTBUG-95925
        this->resize(this->size());
    });
}

bool borderless_window_win::nativeEvent(const QByteArray& eventType, void* message, long* result)
{

    if (eventType == "windows_generic_MSG")
    {
        MSG* msg = static_cast<MSG*>(message);
        if (msg == Q_NULLPTR)
            return false;

        switch (msg->message)
        {
        //DIALOG_DPI_CHANGE_BEHAVIORS
        //if (msg != Q_NULLPTR && msg->message == WM_DPICHANGED)
        //{
        //    //case WM_DPICHANGED: {
        //    // DPI 缩放变化事件发生时执行的代码
        //    // wParam 包含新的 DPI 缩放系数
        //    // lParam 包含一个指向新的窗口位置的 RECT 结构
        //    // 你可以在这里处理 DPI 缩放变化，例如重新布局窗口内容
        //    // 或者重新计算和缩放窗口中的控件
        //    // 你还可以使用以下函数来处理 DPI 缩放变化：
        //    // SetWindowPos() 重新调整窗口大小和位置
        //    // GetDpiForWindow() 获取指定窗口的 DPI 缩放系数
        //    // GetDeviceCaps() 获取设备的 DPI 缩放系数
        //    // 最后，告诉系统你已经处理了 DPI 缩放事件
        //    return TRUE;

        //}

        case WM_COMMAND: {
            SendMessage(msg->hwnd, WM_SYSCOMMAND, msg->wParam, msg->lParam);
            *result = DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
            return true;
        }
        case WM_SHOWWINDOW: {
            bool iswin7classic = false;
            BOOL composition_enabled = FALSE;
            bool success = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
            iswin7classic = !(composition_enabled && success);
            //            auto hasTitle = (GetWindowLongPtr(msg->hwnd, GWL_STYLE) & WS_CAPTION) > 0;
            //            if (hasTitle)
            SetWindowLongPtr(msg->hwnd, GWL_STYLE, WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN);
            return false;
        }

        case WM_NCCALCSIZE: {
            NCCALCSIZE_PARAMS& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
            if (params.rgrc[0].top != 0)
                params.rgrc[0].top -= 1;

            //this kills the window frame and title bar we added with WS_THICKFRAME and WS_CAPTION
            *result = WVR_REDRAW;
            return true;
        }
        case WM_NCHITTEST: {
            const LONG borderWidth = 8; //in pixels
            RECT winrect;
            GetWindowRect(msg->hwnd, &winrect);
            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);

            // caption, a.k.a. title bar
            auto wndScaleFactor = qgetenv("QT_SCALE_FACTOR").toDouble();

            if (x >= winrect.left + borderWidth && x < winrect.right - titleBarBtnWidth() * wndScaleFactor &&
                    y > winrect.top + borderWidth && y < winrect.top + titleHeight() * wndScaleFactor)
            {
                *result = HTCAPTION;
                return true;
            }
            //bottom left corner
            if (x >= winrect.left && x < winrect.left + borderWidth &&
                    y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOMLEFT;
                return true;
            }
            //bottom right corner
            if (x < winrect.right && x >= winrect.right - borderWidth &&
                    y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOMRIGHT;
                return true;
            }
            //top left corner
            if (x >= winrect.left && x < winrect.left + borderWidth &&
                    y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOPLEFT;
                return true;
            }
            //top right corner
            if (x < winrect.right && x >= winrect.right - borderWidth &&
                    y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOPRIGHT;
                return true;
            }
            //left border
            if (x >= winrect.left && x < winrect.left + borderWidth)
            {
                *result = HTLEFT;
                return true;
            }
            //right border
            //为了让点中滚动条
            if (x < winrect.right && x >= winrect.right - 1)
            {
                *result = HTRIGHT;
                return true;
            }
            //bottom border
            if (y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOM;
                return true;
            }
            //top border
            if (y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOP;
                return true;
            }
            return false;
        }

        case WM_ACTIVATE: {
            //如果在win7失去焦点需要重新把边框隐藏掉
            if (QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7 && msg->wParam == WA_INACTIVE)
                SetWindowPos(msg->hwnd, Q_NULLPTR, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        }
        case WM_SETFOCUS: {
            //如果在win7失去焦点需要重新把边框隐藏掉
            if (QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7)
                SetWindowPos(msg->hwnd, Q_NULLPTR, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        }
        case WM_ACTIVATEAPP: {
            return true;
        }
        case WM_NCACTIVATE: {
            //     不返回 flase  ,子窗口非客户区会点击 不了,返回true,  标题栏会在切换窗口时抖动
            if (!::IsIconic(msg->hwnd) && ::IsWindowEnabled(msg->hwnd)) //如果窗口不是最小化&&没有模态窗口
                return true;
        }

        default:
            break;
        }
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

}// namespace prism::qt::ui
