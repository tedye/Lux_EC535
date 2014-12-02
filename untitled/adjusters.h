#ifndef ADJUSTERS_H
#define ADJUSTERS_H

#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QString>


class Adjusters : public QVBoxLayout
{
    Q_OBJECT

public:
    Adjusters(QString text,double upper, double lower);
    ~Adjusters();

private:
    QPushButton *top;
    QPushButton *bottom;
    QTextEdit *text;
    double upperLimit;
    double lowerLimit;
};

#endif // ADJUSTERS_H
