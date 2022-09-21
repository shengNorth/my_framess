#include "widget.h"
#include "ui_widget.h"
#include <QTimer>

#include <QGraphicsDropShadowEffect>

Widget::Widget(QWidget *parent)
    : ShadowWindow<QWidget>(parent, 6)
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

