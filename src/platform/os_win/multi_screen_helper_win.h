#ifndef PRISM_QT_UI_MULTI_SCREEN_HELPER_WIN_H
#define PRISM_QT_UI_MULTI_SCREEN_HELPER_WIN_H

#include <QVector>
#include <Windows.h>

namespace prism::qt::ui {

class multi_screen_helper_win
{
  public:
    typedef struct screen_info_
    {
        screen_info_()
        {
            ZeroMemory(&originInfo, sizeof(originInfo));
            originInfo.dmSize = sizeof(DEVMODE);
        }

        int sysPosX = 0;
        int sysPosY = 0;
        int sysWidth = 0;
        int sysHeight = 0;
        int physicalPosX = 0;
        int physicalPosY = 0;
        int physicalWidth = 0;
        int physicalHeight = 0;
        int logicalPixels = 0;
        int bitsPerPel = 32;
        qreal scalingFactor = 0;
        bool isPrimaryScreen = false;
        QString deviceName = QString();
        QString firendlyName = QString();
        DEVMODE originInfo;
    } screen_info;

  public:
    multi_screen_helper_win();
    ~multi_screen_helper_win();

  public:
    static QVector<screen_info> getAllScreensInfo();
    static screen_info getPrimaryScreenInfo();
    static void printAllScreenInfo();

  private:
    static BOOL updateAllScreenInfo();

    static BOOL CALLBACK callbackGetScreenInfo(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam);
};

}// namespace prism::qt::ui

#endif // PRISM_QT_UI_MULTI_SCREEN_HELPER_WIN_H
