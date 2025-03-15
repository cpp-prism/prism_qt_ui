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

    NSUInteger styleMask = [nativeWindow styleMask];

#if MAC_OS_X_VERSION_MAX_ALLOWED >= 140000
    styleMask |= NSWindowStyleMaskFullSizeContentView;
#else
    styleMask |= NSFullSizeContentViewWindowMask  ;
    styleMask |= NSWindowStyleMaskFullSizeContentView  ;

#endif

    [nativeWindow setStyleMask:styleMask];

#if MAC_OS_X_VERSION_MAX_ALLOWED >= 140000
    [nativeWindow setTitlebarAppearsTransparent:YES];
    [nativeWindow setMovableByWindowBackground:NO];
#else
    //[nativeWindow setTitleVisibility:NSWindowTitleHidden];
    [nativeWindow setTitlebarAppearsTransparent:YES];
    [nativeWindow setMovableByWindowBackground:NO];

#endif
}


} // namespace prism::qt::ui
