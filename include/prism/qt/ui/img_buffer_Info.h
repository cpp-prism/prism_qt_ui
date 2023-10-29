#ifndef PRISM_QT_UI_OPENGL_RENDER_INFO_H
#define PRISM_QT_UI_OPENGL_RENDER_INFO_H
#include "prismQt_ui_global.h"
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>
#include <deque>

namespace prism::qt::ui
{

struct PRISMQT_UI_EXPORT img_buffer_Info{
    double height;
    double width;
    int pixelType;
    int frameNum;
    bool doFreeOpenGL = false;
    std::deque<std::shared_ptr<unsigned char>> buffers;
    std::mutex buffer_mux;
    std::condition_variable buffer_cv;

    static std::map<std::string ,std::shared_ptr<img_buffer_Info>> infos;
};

}// namespace prism::qt::ui

#endif // PRISM_QT_UI_OPENGL_RENDER_INFO_H
