#ifndef DATEPICKERCPP_H
#define DATEPICKERCPP_H

#include <QQuickItem>
#include <QDateTime>

class DatePickerCpp : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QDateTime selectedTime READ selectedTime WRITE setSelectedTime NOTIFY selectedTimeChanged)
    Q_PROPERTY(bool refreshDate READ refreshDate WRITE setRefreshDate NOTIFY refreshDateChanged)
    Q_PROPERTY(bool refreshMonth READ refreshMonth WRITE setRefreshMonth NOTIFY refreshMonthChanged)

public:
    DatePickerCpp();

    QDateTime selectedTime() const;
    Q_INVOKABLE void setSelectedTime(const QDateTime &newSelectedTime);

    bool refreshDate() const;
    void setRefreshDate(bool newRefreshDate);
    Q_INVOKABLE void forceRefreshData();
    Q_INVOKABLE void forceRefreshMonth();

    bool refreshMonth() const;
    void setRefreshMonth(bool newRefreshMonth);

signals:

    void selectedTimeChanged();
    void refreshDateChanged();

    void refreshMonthChanged();

private:
    QDateTime m_selectedTime;
    bool m_refreshDate = true;
    bool m_refreshMonth;
};

#endif // DATEPICKERCPP_H
