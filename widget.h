#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "framelesshelper.h"
#include "ShadowWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public CustomWindowWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


    void mousePressEvent(QMouseEvent* event) override;
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
