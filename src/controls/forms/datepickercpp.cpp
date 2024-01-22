#include "datepickercpp.h"

DatePickerCpp::DatePickerCpp()
{

}

QDateTime DatePickerCpp::selectedTime() const
{
    return m_selectedTime;
}

void DatePickerCpp::setSelectedTime(const QDateTime &newSelectedTime)
{
    if (m_selectedTime == newSelectedTime)
        return;
    m_selectedTime = newSelectedTime;
    emit selectedTimeChanged();
}

bool DatePickerCpp::refreshDate() const
{
    return m_refreshDate;
}

void DatePickerCpp::setRefreshDate(bool newRefreshDate)
{
    if (m_refreshDate == newRefreshDate)
        return;
    m_refreshDate = newRefreshDate;
    emit refreshDateChanged();
}

void DatePickerCpp::forceRefreshData()
{
    setRefreshDate(!refreshDate());
}

void DatePickerCpp::forceRefreshMonth()
{
    setRefreshMonth(!refreshMonth());
}

bool DatePickerCpp::refreshMonth() const
{
    return m_refreshMonth;
}

void DatePickerCpp::setRefreshMonth(bool newRefreshMonth)
{
    if (m_refreshMonth == newRefreshMonth)
        return;
    m_refreshMonth = newRefreshMonth;
    emit refreshMonthChanged();
}
