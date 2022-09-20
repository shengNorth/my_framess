#include "widget.h"
#include "ui_widget.h"
#include <QTimer>

#include <QGraphicsDropShadowEffect>

Widget::Widget(QWidget *parent)
    : MuShadowWindow<QWidget>(true, 12, parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this->clientWidget());
    this->resize(800,600);
}

Widget::~Widget()
{
    delete ui;
}

