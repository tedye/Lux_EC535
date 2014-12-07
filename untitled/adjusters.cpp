#include "adjusters.h"
#include <QIcon>
#include <QPixmap>
#include <math.h>
#include <stdio.h>

Adjusters::Adjusters(QString text):QVBoxLayout()
{
    // initialization
    this->top = new QPushButton(QIcon(QPixmap(":/Images/up.png")),"",0);
    this->bottom = new QPushButton(QIcon(QPixmap(":/Images/down.png")),"",0);
    this->text = new QLabel(text);
    this->initValues(text);
    this->whoami = text;

    // adding widgets
    this->addWidget(top);
    this->addWidget(this->text);
    this->addWidget(bottom);

    // styling
    top->setIconSize(QSize(50,50));
    bottom->setIconSize(QSize(50,50));
    top->setStyleSheet("QPushButton{border: none; outline: none;}");
    bottom->setStyleSheet("QPushButton{border: none; outline: none;}");
    bottom->setFixedWidth(20);
    top->setFixedWidth(20);
    this->setAlignment(bottom,Qt::AlignHCenter);
    this->setAlignment(top,Qt::AlignHCenter);
    this->setAlignment(this->text,Qt::AlignHCenter);


    // connecting our signal handlers
    connect(this->top,SIGNAL(released()),this,SLOT(handleUp()));
    connect(this->bottom,SIGNAL(released()),this,SLOT(handleDown()));
    
}

Adjusters::~Adjusters()
{
    delete top;
    delete bottom;
    delete text;
}

void Adjusters::initValues(QString text)
{
    if(text == "iso")
    {
        for(double i = 100.0; i <= 3200.0; i = i*2.0)
        {
            values.push_back(i);
        }
    }
    else if(text == "aperture")
    {
        for(double i = 1.0; i <= 9.0; i += 0.25)
        {
            values.push_back(pow(1.41,i));
        }
    }
    else if(text == "shutter")
    {
        for(int i = 4000; i >=1; i = i/2)
        {
            values.push_back((double)i);
        }
    }
    else if(text == "exposure")
    {
        for(double i = -3.0; i <= 3.0; i++)
        {
            values.push_back(i);
        }
    }
    char sample[30];
    sprintf(sample,"%1g",values.front());
    this->text->setText(QString(sample));
    this->pos = values.begin();
    current = values.front();
}


void Adjusters::handleUp()
{
    char temp[30];
    if(pos != (--values.end()))
    {
        sprintf(temp,"%1g",*(++pos));
        this->text->setText(QString(temp));
    }
    current = *pos;
}

void Adjusters::handleDown()
{
    char temp[30];
    if(pos != values.begin())
    {
        sprintf(temp,"%1g",*(--pos));
        this->text->setText(QString(temp));
    }
    current = *pos;
}
