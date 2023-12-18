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

enum  class ENUM_PixelType
{
    mono8,
    rgb8,
    bayerBG8,
    bayerGB8,
    bayerGR8,
    bayerRG8

};

struct PRISMQT_UI_EXPORT img_frame_info{

    double height;
    double width;
    int frameNum;
    ENUM_PixelType pixelType = ENUM_PixelType::mono8;
    std::shared_ptr<unsigned char> buffer;
};

struct PRISMQT_UI_EXPORT img_buffer_Info{

    img_frame_info pre_frame;
    std::deque<img_frame_info> frames;

    bool doFreeOpenGL = false;

    std::mutex buffer_mux;
    std::condition_variable buffer_cv;

    static std::map<std::string ,std::shared_ptr<img_buffer_Info>> infos;
};

}// namespace prism::qt::ui

#endif // PRISM_QT_UI_OPENGL_RENDER_INFO_H
