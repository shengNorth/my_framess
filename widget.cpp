#include "widget.h"
#include "ui_widget.h"
#include <QTimer>

#include <QGraphicsDropShadowEffect>

Widget::Widget(QWidget *parent)
    : MuShadowWindow<QWidget>(parent, 6)
    , ui(new Ui::Widget)
{
    ui->setupUi(this->GetClientWidget());
    this->resize(800,600);

//    this->SetResizeable(false);
    this->SetMoveable(true);
}

Widget::~Widget()
{
    delete ui;
}

