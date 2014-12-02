#include "adjusters.h"
#include <QIcon>
#include <QPixmap>
Adjusters::Adjusters(QString text,double upper, double lower):QVBoxLayout()
{
    top = new QPushButton(QIcon(QPixmap(":/Images/up.png")),"",0);
    bottom = new QPushButton(QIcon(QPixmap(":/Images/down.png")),"",0);
    this->text = new QTextEdit(text);
    //this->setSpacing(10);
    this->addWidget(top);
    this->addWidget(this->text);
    this->addWidget(bottom);
    top->setIconSize(QSize(50,50));
    bottom->setIconSize(QSize(50,50));
    top->setStyleSheet("QPushButton{border: none; outline: none;}");
    bottom->setStyleSheet("QPushButton{border: none; outline: none;}");
    bottom->setFixedWidth(20);
    top->setFixedWidth(20);
    this->text->setReadOnly(true);
    this->text->setFixedSize(QSize(80,20));
    //this->text->setDisabled(true);
    this->setAlignment(bottom,Qt::AlignHCenter);
    this->setAlignment(top,Qt::AlignHCenter);
    this->setAlignment(this->text,Qt::AlignHCenter);

    this->upperLimit = upper;
    this->lowerLimit = lower;
    //this->SetFixedSize;
}

Adjusters::~Adjusters()
{
    delete top;
    delete bottom;
}
