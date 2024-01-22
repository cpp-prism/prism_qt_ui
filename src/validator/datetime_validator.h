#ifndef DATETIME_VALIDATOR_H
#define DATETIME_VALIDATOR_H

#include <QValidator>
#include <QDateTime>

class datetime_validator : public QValidator
{
public:
    explicit datetime_validator();

signals:


    // QValidator interface
public:
    State validate(QString &, int &) const override;
};

#endif // DATETIME_VALIDATOR_H
