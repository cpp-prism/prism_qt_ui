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

enum class PRISMQT_UI_EXPORT ENUM_PixelType
{
    mono8 = 0,
    rgb24,
    rgba32,
    bgr24,
    bgra32,
    bayerBG8,//not implemented
    bayerGB8,//not implemented
    bayerGR8,//not implemented
    bayerRG8,//not implemented
    nv12,//not implemented
    yuv420p//not implemented

};

struct PRISMQT_UI_EXPORT ImgFrameInfo{

    int height = 480;
    int width = 640;
    int stride =640;

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


class PRISMQT_UI_EXPORT ImgFrameQueue {
public:
    explicit ImgFrameQueue(size_t maxSize = 3);

    void push(const ImgFrameInfo& frame);

    bool tryPopLatest(ImgFrameInfo& out);
    bool tryPopLatest_do(std::function<void(ImgFrameInfo& info)> f);
    bool tryPeekLatest(ImgFrameInfo& out);
    bool tryPeekLatest_do(std::function<void(ImgFrameInfo& info)> f);

    bool waitPop(ImgFrameInfo& out, int timeoutMs = -1);

    void clear();

    size_t size() const;

    static std::map<std::string ,std::shared_ptr<ImgFrameQueue>> queues;
    static std::shared_ptr<ImgFrameQueue> getQueue(std::string sn);
private:
    size_t m_maxSize = 3;
    std::deque<ImgFrameInfo> m_frames;

    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
};



}// namespace prism::qt::ui

PRISM_ENUM(prism::qt::ui::ENUM_PixelType,{{ prism::qt::ui::ENUM_PixelType::mono8,"mono8"},
                                          { prism::qt::ui::ENUM_PixelType::rgb24,"rgb24"},
                                          { prism::qt::ui::ENUM_PixelType::rgba32,"rgba32"},
                                          { prism::qt::ui::ENUM_PixelType::bgr24,"bgr24"},
                                          { prism::qt::ui::ENUM_PixelType::bgra32,"bgra32"},
                                          { prism::qt::ui::ENUM_PixelType::bayerBG8,"bayerBG8"},
                                          { prism::qt::ui::ENUM_PixelType::bayerGB8,"bayerGB8"},
                                          { prism::qt::ui::ENUM_PixelType::bayerGR8,"bayerGR8"},
                                          { prism::qt::ui::ENUM_PixelType::bayerRG8,"bayerRG8"},
                                          { prism::qt::ui::ENUM_PixelType::nv12,"nv12"},
                                          { prism::qt::ui::ENUM_PixelType::yuv420p,"yuv420p"} });
PRISMQT_ENUM(prism::qt::ui::ENUM_PixelType);

PRISM_FIELDS(prism::qt::ui::ImgFrameInfo,height,width,stride,frameNum,pixelType,receivedBufferNum,incompleteBufferNum,buffersPerSecond,detectedBufferNum);
PRISMQT_CLASS(prism::qt::ui::ImgFrameInfo);
#endif // PRISM_QT_UI_OPENGL_RENDER_INFO_H
