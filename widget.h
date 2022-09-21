#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "framelesshelper.h"
#include "ShadowWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public ShadowWindow<QWidget>
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    FramelessHelper*    m_pHelper = nullptr;
};
#endif // WIDGET_H
