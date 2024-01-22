#include "datetime_validator.h"
#include <regex>
#include <sstream>

datetime_validator::datetime_validator() : QValidator()
{

}

QValidator::State datetime_validator::validate(QString &input, int &) const
{
    QDateTime dt = QDateTime::fromString(input, "yyyy/MM/dd HH:mm:ss");
    if(dt.isNull())
        return QValidator::Invalid;
    else
        return QValidator::Acceptable;
    //std::regex pattern(R"((\d{4,4})/(\d{2,2})/(\d{2,2}) (\d{2,2}):(\d{2,2}):(\d{2,2}))");

    //std::smatch match;
    //std::string inputstr = input.toStdString();
    //if(std::regex_search(inputstr,match,pattern))
    //{
    //    int year = 2001;
    //    int mouth;
    //    int day;
    //    int hour;
    //    int min;
    //    int second;
    //    hour =  std::atoi(match.str(1).c_str());
    //    mouth =  std::atoi(match.str(2).c_str());
    //    day =  std::atoi(match.str(3).c_str());
    //    hour =  std::atoi(match.str(4).c_str());
    //    min =  std::atoi(match.str(5).c_str());
    //    second =  std::atoi(match.str(6).c_str());

    //    QDate date(year,mouth,day);
    //    if(date.isNull() || !date.isValid())
    //        return QValidator::Invalid;

    //    QTime time(hour,min,second);
    //    if(time.isNull() || !time.isValid())
    //        return QValidator::Invalid;

    //    return QValidator::Acceptable;
    //}
    //else
    //    return QValidator::Invalid;
}
