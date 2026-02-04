#include "include/prism/qt/ui/img_buffer_Info.h"
namespace prism::qt::ui
{


ImgFrameQueue::ImgFrameQueue(size_t maxSize)
    : m_maxSize(maxSize)
{

}

void ImgFrameQueue::push(const ImgFrameInfo &frame) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_frames.size() >= m_maxSize) {
            m_frames.pop_front();
        }

        m_frames.push_back(frame);
    }
    m_cv.notify_one();
}

bool ImgFrameQueue::tryPopLatest(ImgFrameInfo &out) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_frames.empty())
        return false;

    out = std::move(m_frames.back());
    m_frames.clear();   // 关键：丢掉旧帧
    return true;
}

bool ImgFrameQueue::waitPop(ImgFrameInfo &out, int timeoutMs) {
    std::unique_lock<std::mutex> lock(m_mutex);

    if (timeoutMs < 0) {
        m_cv.wait(lock, [this] { return !m_frames.empty(); });
    } else {
        if (!m_cv.wait_for(lock,
                           std::chrono::milliseconds(timeoutMs),
                           [this] { return !m_frames.empty(); }))
            return false;
    }

    out = std::move(m_frames.back());
    m_frames.clear();
    return true;
}

void ImgFrameQueue::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_frames.clear();
}

size_t ImgFrameQueue::size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_frames.size();
}

std::shared_ptr<ImgFrameQueue> ImgFrameQueue::getQueue(std::string sn)
{
    std::shared_ptr<ImgFrameQueue> q = queues[sn];
    if(!q)
    {
        queues[sn].reset(new ImgFrameQueue());
        q=queues[sn];
    }
    return q;
}


std::map<std::string, std::shared_ptr<ImgFrameQueue> > ImgFrameQueue::queues{};


}
