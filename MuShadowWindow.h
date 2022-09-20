/**
 * 自定义无边框窗体、对话框和提示框
 *
 * MuShadowWindow.h
 * 所有自定义窗口的基类，定义窗口各个位置的布局
 *
 * FlyWM_
 * GitHub: https://github.com/FlyWM
 * CSDN: https://blog.csdn.net/a844651990
 *
 */

#ifndef MUSHADOWWINDOW_H
#define MUSHADOWWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QPushButton>
#include <QDialog>
#include <QMessageBox>
#include <QResizeEvent>
#include <QPixmap>
#include "titlebar.h"
#include "framelesshelper.h"

class QPainter;
class QLineEdit;
class QLabel;

//继承自QDialog,将声明和实现分开
//使用背景图做为阴影
//使用framelessHalper来处理无边框, 按住标题栏移动等事件
//最大化后,拖动标题栏还原,在framelessHelper中处理

template <class Base = QWidget>
class MuShadowWindow : public Base
{
public:
    explicit MuShadowWindow(bool canResize, int shadowSize, QWidget *parent = nullptr)
        : Base(parent)
    {
        m_shadowSize= 6;
        Base *pT = this;
        pT->setAttribute(Qt::WA_TranslucentBackground, true);
        pT->setWindowFlags(Qt::FramelessWindowHint);
        pT->setContentsMargins(0, 0, 0, 0);

        m_vBoxLayout = new QVBoxLayout(pT);
        m_vBoxLayout->setContentsMargins(m_shadowSize, m_shadowSize, m_shadowSize, m_shadowSize);
        m_vBoxLayout->setSpacing(0);

        // 窗口主要区域
        m_titleBar =  new MuTitleBar(this, this, canResize);
        this->installEventFilter(m_titleBar);
        m_titleBar->setObjectName("titleBar");
        m_vBoxLayout->addWidget(m_titleBar);

        //CenterWidget
        m_pClientWidget = new QWidget(this);
        m_pClientWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_vBoxLayout->addWidget(m_pClientWidget);

        this->setLayout(m_vBoxLayout);


        m_pHelper = new FramelessHelper(this);
        m_pHelper->activateOn(this);
        m_pHelper->setTitleHeight(m_titleBar->height());
        m_pHelper->setWidgetResizable(true);
        m_pHelper->setWidgetMovable(true);
        m_pHelper->setRubberBandOnMove(false);
        m_pHelper->setRubberBandOnResize(false);


        QObject::connect(m_titleBar, &MuTitleBar::HeightChanged, [this](const int &height) { m_pHelper->setTitleHeight(height); });
    }

public:
    QPixmap NinePatchScalePixmap(QString picName, int iHorzSplit, int iVertSplit, int DstWidth, int DstHeight)
    {
        QPixmap* pix = new QPixmap(picName);

        int pixWidth = pix->width();
        int pixHeight = pix->height();

        QPixmap pix_1 = pix->copy(0, 0, iHorzSplit, iVertSplit);
        QPixmap pix_2 = pix->copy(iHorzSplit, 0, pixWidth - iHorzSplit * 2, iVertSplit);
        QPixmap pix_3 = pix->copy(pixWidth - iHorzSplit, 0, iHorzSplit, iVertSplit);

        QPixmap pix_4 = pix->copy(0, iVertSplit, iHorzSplit, pixHeight - iVertSplit * 2);
        QPixmap pix_5 = pix->copy(iHorzSplit, iVertSplit, pixWidth - iHorzSplit * 2, pixHeight - iVertSplit * 2);
        QPixmap pix_6 = pix->copy(pixWidth - iHorzSplit, iVertSplit, iHorzSplit, pixHeight - iVertSplit * 2);

        QPixmap pix_7 = pix->copy(0, pixHeight - iVertSplit, iHorzSplit, iVertSplit);
        QPixmap pix_8 = pix->copy(iHorzSplit, pixHeight - iVertSplit, pixWidth - iHorzSplit * 2, pixWidth - iHorzSplit * 2);
        QPixmap pix_9 = pix->copy(pixWidth - iHorzSplit, pixHeight - iVertSplit, iHorzSplit, iVertSplit);

        pix_2 = pix_2.scaled(DstWidth - iHorzSplit * 2, iVertSplit, Qt::IgnoreAspectRatio);//保持高度拉宽;
        pix_4 = pix_4.scaled(iHorzSplit, DstHeight - iVertSplit * 2, Qt::IgnoreAspectRatio);//保持宽度拉高;
        pix_5 = pix_5.scaled(DstWidth - iHorzSplit * 2, DstHeight - iVertSplit * 2, Qt::IgnoreAspectRatio);//宽高都缩放;
        pix_6 = pix_6.scaled(iHorzSplit, DstHeight - iVertSplit * 2, Qt::IgnoreAspectRatio);//保持宽度拉高;
        pix_8 = pix_8.scaled(DstWidth - iHorzSplit * 2, iVertSplit);//保持高度拉宽;


        QPixmap resultImg(DstWidth, DstHeight);
        // 需设置背景透明;
        resultImg.fill(Qt::transparent);
        QPainter* painter = new QPainter(&resultImg);
        if (!resultImg.isNull()) {
            painter->drawPixmap(0, 0, pix_1);
            painter->drawPixmap(iHorzSplit, 0, pix_2);
            painter->drawPixmap(DstWidth - iHorzSplit, 0, pix_3);

            painter->drawPixmap(0, iVertSplit, pix_4);
            painter->drawPixmap(iHorzSplit, iVertSplit, pix_5);
            painter->drawPixmap(DstWidth - iHorzSplit, iVertSplit, pix_6);

            painter->drawPixmap(0, DstHeight - iVertSplit, pix_7);
            painter->drawPixmap(iHorzSplit, DstHeight - iVertSplit, pix_8);
            painter->drawPixmap(DstWidth - iHorzSplit, DstHeight - iVertSplit, pix_9);
            painter->end();
        }
        return resultImg;
    }

    // 每次窗口发生变化，就重新生成一张背景图，适用于可拉伸的窗口，也适用于未指定大小的窗口;
    void resizeEvent(QResizeEvent *event)
    {
        m_shadowBackPixmap = NinePatchScalePixmap(":/button/border_shadow.png",
            m_shadowSize, m_shadowSize, this->width(), this->height());
        return QWidget::resizeEvent(event);
    }


    // 直接绘制阴影边框即可;
    void paintEvent(QPaintEvent *event)
    {
        QPainter painter(this);
        painter.drawPixmap(this->rect(), m_shadowBackPixmap);
        // 绘制白色背景,因为窗口背景设置为透明;
        painter.fillRect(this->rect().adjusted(m_shadowSize, m_shadowSize, -m_shadowSize, -m_shadowSize),
                         QBrush(Qt::white));
    }
    QWidget *clientWidget() const { return m_pClientWidget; }
    MuTitleBar *titleBar() const { return m_titleBar; }
    void setRubberBandOnMove(bool enable) { m_pHelper->setRubberBandOnMove(enable); }
    void setRubberBandOnResize(bool enable) { m_pHelper->setRubberBandOnResize(enable); }
    void setTitleBarHeight(int height)
    {
        if (height < 0)
            return;

        m_titleBar->setFixedHeight(height);
        m_pHelper->setTitleHeight(height);
    }

    void setResizable(bool resizable) { m_pHelper->setWidgetResizable(resizable); }
    void setMovable(bool movable) {m_pHelper->setWidgetMovable(movable); }

private:
    int m_borderSize;
    int m_shadowSize;
    QWidget *m_pClientWidget;
    QVBoxLayout *m_vBoxLayout;
    FramelessHelper*    m_pHelper;
    MuTitleBar*     m_titleBar;
    QPixmap         m_shadowBackPixmap;
};

typedef MuShadowWindow<QWidget> MuCustomWindowWidget;
typedef MuShadowWindow<QDialog> MuCustomDialogWidget;
typedef MuShadowWindow<QMessageBox> MuCustomMessageBoxWidget;

#endif // MUSHADOWWINDOW_H
