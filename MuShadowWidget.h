/**
 * 自定义无边框窗体、对话框和提示框
 *
 * MuShadowWidget.h
 * 实现边框阴影
 *
 * FlyWM_
 * GitHub: https://github.com/FlyWM
 * CSDN: https://blog.csdn.net/a844651990
 *
 */

#ifndef MUSHADOWWIDGET_H
#define MUSHADOWWIDGET_H

#include <QWidget>

class MuSkin9GridImage
{
protected:
    QImage m_img;
    QRect m_arrayImageGrid[9];
    //
    bool clear();
public:
    static bool splitRect(const QRect& rcSrc, QPoint ptTopLeft, QRect* parrayRect, int nArrayCount);
    bool setImage(const QImage& image, QPoint ptTopLeft);
    void drawBorder(QPainter* p, QRect rc) const;
};


class MuShadowWidget : public QWidget
{
    Q_OBJECT
public:
    MuShadowWidget(int shadowSize, QWidget *parent = nullptr);

    virtual void paintEvent(QPaintEvent *e);

private:
    MuSkin9GridImage* m_shadow;
};

#endif // MUSHADOWWIDGET_H
