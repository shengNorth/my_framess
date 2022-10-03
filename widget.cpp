#include "widget.h"
#include "ui_widget.h"
#include <QTimer>

Widget::Widget(QWidget *parent)
    : CustomFrame(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this->GetClientWidget());
    this->resize(800,600);

//    this->SetResizeable(false);
//    this->SetMoveable(false);
//    this->SetHasMaxFun(false);
    this->setWindowTitle("测试程序");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{

}


