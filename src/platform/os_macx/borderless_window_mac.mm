#include "borderless_window_mac.h"
#include <Cocoa/Cocoa.h>
#include <QGuiApplication>
#include <QWindow>

namespace prism::qt::ui
{

borderless_window_mac::borderless_window_mac()
{
}

void borderless_window_mac::enableWindowBorderless(qulonglong wId)
{
    NSView *nativeView = reinterpret_cast<NSView *>(wId);
    NSWindow *nativeWindow = [nativeView window];
    //[nativeWindow setStyleMask:[nativeWindow styleMask] | NSFullSizeContentViewWindowMask | NSWindowTitleHidden];
    [nativeWindow setStyleMask:[nativeWindow styleMask] | NSWindowStyleMaskFullSizeContentView | NSWindowTitleHidden];

    [nativeWindow setTitlebarAppearsTransparent:YES];
    [nativeWindow setMovableByWindowBackground:NO];
}

} // namespace prism::qt::ui
