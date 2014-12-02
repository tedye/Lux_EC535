#ifndef WIDGET_H
#define WIDGET_H

#include "adjusters.h"
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    QHBoxLayout *boxBox;
    Adjusters *iso;
    Adjusters *shutter;
    Adjusters *aperture;
    Adjusters *exposure;
    Ui::Widget *ui;
};

#endif // WIDGET_H
