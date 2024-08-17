#ifndef VIDEO_ROI_CPP_H
#define VIDEO_ROI_CPP_H

#include <QQuickItem>

class video_roi_cpp : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int roi_x READ roi_x WRITE setRoi_x NOTIFY roi_xChanged)
    Q_PROPERTY(int roi_y READ roi_y WRITE setRoi_y NOTIFY roi_yChanged)
    Q_PROPERTY(int roi_width READ roi_width WRITE setRoi_width NOTIFY roi_widthChanged)
    Q_PROPERTY(int roi_height READ roi_height WRITE setRoi_height NOTIFY roi_heightChanged)
    int m_roi_x = 0;

    int m_roi_y = 0;

    int m_roi_width = 0;

    int m_roi_height = 0;

  public:
    video_roi_cpp();

    int roi_x() const;
    Q_INVOKABLE void setRoi_x(int newRoi_x);

    int roi_y() const;
    Q_INVOKABLE void setRoi_y(int newRoi_y);

    int roi_width() const;
    Q_INVOKABLE void setRoi_width(int newRoi_width);

    int roi_height() const;
    Q_INVOKABLE void setRoi_height(int newRoi_height);

  signals:

    void roi_xChanged();
    void roi_yChanged();
    void roi_widthChanged();
    void roi_heightChanged();
};

#endif // VIDEO_ROI_CPP_H
