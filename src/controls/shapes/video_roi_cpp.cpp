#include "video_roi_cpp.h"

video_roi_cpp::video_roi_cpp()
{
}

int video_roi_cpp::roi_x() const
{
    return m_roi_x;
}

void video_roi_cpp::setRoi_x(int newRoi_x)
{
    if (m_roi_x == newRoi_x)
        return;
    m_roi_x = newRoi_x;
    emit roi_xChanged();
}

int video_roi_cpp::roi_y() const
{
    return m_roi_y;
}

void video_roi_cpp::setRoi_y(int newRoi_y)
{
    if (m_roi_y == newRoi_y)
        return;
    m_roi_y = newRoi_y;
    emit roi_yChanged();
}

int video_roi_cpp::roi_width() const
{
    return m_roi_width;
}

void video_roi_cpp::setRoi_width(int newRoi_width)
{
    if (m_roi_width == newRoi_width)
        return;
    m_roi_width = newRoi_width;
    emit roi_widthChanged();
}

int video_roi_cpp::roi_height() const
{
    return m_roi_height;
}

void video_roi_cpp::setRoi_height(int newRoi_height)
{
    if (m_roi_height == newRoi_height)
        return;
    m_roi_height = newRoi_height;
    emit roi_heightChanged();
}
