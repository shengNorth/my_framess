#ifndef SHADOWWINDOW_H
#define SHADOWWINDOW_H


#include <QDesktopWidget>
#include "titlebar.h"
#include "framelesshelper.h"

class QPainter;
class QLineEdit;
class QLabel;


template <class Base = QWidget>
class ShadowWindow : public Base
{
public:
    explicit ShadowWindow(QWidget *parent = nullptr, int shadowSize = 6)
        : Base(parent)
    {
        m_shadowWidth = shadowSize;

        Base *pT = this;
        pT->setAttribute(Qt::WA_TranslucentBackground, true);
        pT->setWindowFlags(Qt::FramelessWindowHint);
        pT->setContentsMargins(0, 0, 0, 0);

        m_vBoxLayout = new QVBoxLayout(pT);
        m_vBoxLayout->setContentsMargins(m_shadowWidth, m_shadowWidth, m_shadowWidth, m_shadowWidth);
        m_vBoxLayout->setSpacing(0);

        //添加标题栏
        m_titleBar =  new QWidget(this);
        QHBoxLayout* titleLayout = new QHBoxLayout(m_titleBar);
        //支持自定义标题栏
        titleLayout->setContentsMargins(0,0,0,0);
        m_titleBar->setLayout(titleLayout);
        m_titleBar->setFixedHeight(m_titleHeight);

        m_vBoxLayout->addWidget(m_titleBar);

        //添加 CenterWidget
        m_pClientWidget = new QWidget(this);
        m_pClientWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_vBoxLayout->addWidget(m_pClientWidget);

        this->setLayout(m_vBoxLayout);

        this->setWindowIcon(QIcon(":/button/close_hover.png"));

        m_pHelper = new FramelessHelper(this);
        m_pHelper->activateOn(this);
        m_pHelper->setWidgetResizable(true);
        m_pHelper->setWidgetMovable(true);
    }

public:
    void setTitleBarHeight(int height)
    {
        if (height < 0)
            return;
        m_titleHeight = height;
        m_titleBar->setFixedHeight(height);
    }

    void setResizable(bool resizable) { m_pHelper->setWidgetResizable(resizable); }
    void setMovable(bool movable) {m_pHelper->setWidgetMovable(movable); }

    virtual void OnBtnMinClicked()
    {
        showMinimized();
    }

    virtual void OnBtnMaxClicked()
    {
        m_bMaximized = true;
        m_restoreRect = geometry();

        int currentScreen = QApplication::desktop()->screenNumber(this);//程序所在的屏幕编号
        QRect rect = QApplication::desktop()->availableGeometry(currentScreen);//程序所在屏幕可用尺寸
        setGeometry(rect.left() - m_shadowWidth, rect.top() - m_shadowWidth,
                    rect.width() + m_shadowWidth * 2, rect.height() + m_shadowWidth * 2);
    }

    virtual void OnBtnRestoreClicked()
    {
        m_bMaximized = false;
        setGeometry(m_restoreRect);
    }

    virtual void OnBtnCloseClicked()
    {
        close();
    }

    bool IsMaxed() const { return m_bMaximized; }
    int GetShadowWidth() const { return m_shadowWidth; }
    int GetBorderWidth() const { return m_borderWidth; }
    int GetTitleHeight() const { return m_titleHeight; }
    QWidget* GetClientWidget()   { return m_pClientWidget; }

    void SetResizeable(bool enable) { m_pHelper->setWidgetResizable(enable); }
    bool GetResizeable() const { return m_pHelper->widgetResizable(); }
    void SetMoveable(bool enable) { m_pHelper->setWidgetMovable(enable); }
    bool GetMoveable() const { return m_pHelper->widgetMovable(); }
    void SetHasMaxFun(bool has){ m_bHasMaxFun = has; ShowMaxBtn(has);}
    bool GetHasMaxFun() const { return m_bHasMaxFun; }

    /**
     * @brief GetTitleLayout    获取标题栏的水平布局, 用于完全自定义标题栏
     * @return
     */
    QLayout* GetTitleLayout() { return m_titleBar->layout(); }

protected:
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
    virtual void resizeEvent(QResizeEvent *event) override
    {
        m_shadowBackPixmap = NinePatchScalePixmap(":/button/border_shadow.png",
                                                  m_shadowWidth, m_shadowWidth, this->width(), this->height());
        return QWidget::resizeEvent(event);
    }

    virtual void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.drawPixmap(this->rect(), m_shadowBackPixmap);
        // 绘制白色背景,因为窗口背景设置为透明;
        painter.fillRect(this->rect().adjusted(m_shadowWidth, m_shadowWidth, -m_shadowWidth, -m_shadowWidth),
                         QBrush(Qt::white));
    }

private:
    bool        m_bMaximized = false;       //当前是否为最大化状态
    bool        m_bHasMaxFun = true;        //是否有最大化功能
    int         m_borderWidth = 3;
    int         m_shadowWidth = 20;
    int         m_titleHeight = 32;


    QRect               m_restoreRect;              //restore rect
    QWidget*            m_titleBar = nullptr;
    QWidget*            m_pClientWidget = nullptr;
    QPixmap             m_shadowBackPixmap;
    QVBoxLayout*        m_vBoxLayout = nullptr;
    FramelessHelper*    m_pHelper = nullptr;
};

class CustomFrame : public ShadowWindow<QWidget>
{
public:
    CustomFrame(QWidget* parent = nullptr, int shadowWidth = 6):
        ShadowWindow<QWidget>(parent, shadowWidth)
    {
        m_titleBar = new CustomTitleBar(this);
        this->installEventFilter(m_titleBar);
        m_titleBar->setObjectName("titleBar");
        this->GetTitleLayout()->addWidget(m_titleBar);
    }

    CustomFrame(const CustomFrame& ) = delete;

public:
    //是否显示最大化最小化按钮
    void ShowMinBtn(bool show) { m_titleBar->setMinimumVisible(show);}
    void ShowMaxBtn(bool show) { m_titleBar->setMaximumVisible(show);}

    //获取自定义标题栏
    CustomTitleBar* GetTitleBar() { return m_titleBar; }

private:
    CustomTitleBar* m_titleBar = nullptr;
};

using CustomWindow = ShadowWindow<QWidget>;
using CustomDialog = ShadowWindow<QDialog> ;
using CustomMessageBox = ShadowWindow<QMessageBox>;

#endif // SHADOWWINDOW_H
