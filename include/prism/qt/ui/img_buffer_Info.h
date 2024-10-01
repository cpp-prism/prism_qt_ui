#ifndef PRISM_QT_UI_OPENGL_RENDER_INFO_H
#define PRISM_QT_UI_OPENGL_RENDER_INFO_H
#include "prismQt_ui_global.h"
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <prism/qt/core/hpp/prismQt.hpp>

namespace prism::qt::ui
{

enum  class ENUM_PixelType
{
    mono8,
    rgb8,
    bgr8,
    bayerBG8,//not implemented
    bayerGB8,//not implemented
    bayerGR8,//not implemented
    bayerRG8//not implemented

};

struct PRISMQT_UI_EXPORT img_frame_info{

    double height = 1.0;
    double width = 1.0;
    int frameNum = 0;
    ENUM_PixelType pixelType = ENUM_PixelType::mono8;
    void* buffer = nullptr;
    std::shared_ptr<void> buffer_handel;
    //上位机统计
    int receivedBufferNum=0;
    int incompleteBufferNum =0;
    double buffersPerSecond = 0.0;
    int detectedBufferNum = 0;
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

PRISM_ENUM(prism::qt::ui::ENUM_PixelType,{{ prism::qt::ui::ENUM_PixelType::mono8,"mono8"},
                                          { prism::qt::ui::ENUM_PixelType::rgb8,"rgb8"},
                                          { prism::qt::ui::ENUM_PixelType::bgr8,"bgr8"},
                                          { prism::qt::ui::ENUM_PixelType::bayerBG8,"bayerBG8"},
                                          { prism::qt::ui::ENUM_PixelType::bayerGB8,"bayerGB8"},
                                          { prism::qt::ui::ENUM_PixelType::bayerGR8,"bayerGR8"},
                                          { prism::qt::ui::ENUM_PixelType::bayerRG8,"bayerRG8"} });
PRISMQT_ENUM(prism::qt::ui::ENUM_PixelType);

PRISM_FIELDS(prism::qt::ui::img_frame_info,height,width,frameNum,pixelType,receivedBufferNum,incompleteBufferNum,buffersPerSecond,detectedBufferNum);
PRISMQT_CLASS(prism::qt::ui::img_frame_info);
#endif // PRISM_QT_UI_OPENGL_RENDER_INFO_H
