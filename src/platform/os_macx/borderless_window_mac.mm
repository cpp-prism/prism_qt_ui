#include "windowborderlesshelper_macx.h"
#include <Cocoa/Cocoa.h>
#include <QGuiApplication>
#include <QWindow>

WindowBorderlessHelper_macx::WindowBorderlessHelper_macx()
{
}

void WindowBorderlessHelper_macx::enableWindowBorderless(qulonglong wId)
{
    NSView *nativeView = reinterpret_cast<NSView *>(wId);
    NSWindow *nativeWindow = [nativeView window];
    //[nativeWindow setStyleMask:[nativeWindow styleMask] | NSFullSizeContentViewWindowMask | NSWindowTitleHidden];
    [nativeWindow setStyleMask:[nativeWindow styleMask] | NSWindowStyleMaskFullSizeContentView | NSWindowTitleHidden];

    [nativeWindow setTitlebarAppearsTransparent:YES];
    [nativeWindow setMovableByWindowBackground:YES];
}
