#include "widget.h"
#include "ui_widget.h"
#include <QIcon>
#include <QPixmap>
#include "adjusters.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    iso = new Adjusters("iso",100,3200);
    shutter = new Adjusters("shutter",1000,1);
    aperture = new Adjusters("aperture",1,20);
    exposure = new Adjusters("exposure",-3,3);
    boxBox = new QHBoxLayout(this);
    boxBox->addLayout(iso);
    boxBox->addLayout(shutter);
    boxBox->addLayout(aperture);
    boxBox->addLayout(exposure);
    this->setStyleSheet("background-color: white;");
}

Widget::~Widget()
{
    delete ui;
}
