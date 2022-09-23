/**
 * 自定义无边框窗体、对话框和提示框
 *
 * CustomTitleBar.cpp
 * 自定义窗体的标题栏。
 *
 * FlyWM_
 * GitHub: https://github.com/FlyWM
 * CSDN: https://blog.csdn.net/a844651990
 *
 */

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#ifdef Q_OS_WIN32
#include <qt_windows.h>
#endif
#include <QDesktopWidget>
#include <QStyleOption>
#include <QPainter>
#include "titlebar.h"
#include "framelesshelper.h"
#include "ShadowWindow.h"

CustomTitleBar::CustomTitleBar(QWidget *parent, QWidget *window)
    : QWidget(parent)
{
    m_pWindow = static_cast<ShadowWindow<QWidget>*>(window);
    m_pIconLabel = new QLabel(this);
    m_pTitleLabel = new QLabel(this);
    m_pMinimizeButton = new QPushButton(this);
    m_pMaximizeButton = new QPushButton(this);
    m_pCloseButton = new QPushButton(this);
    m_pCustomWidget = new QWidget(this);

    m_pIconLabel->setFixedSize(20, 20);
    m_pIconLabel->setScaledContents(true);

//    m_pTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_pMinimizeButton->setFixedSize(27, 22);
    m_pMaximizeButton->setFixedSize(27, 22);
    m_pCloseButton->setFixedSize(27, 22);

    m_pMinimizeButton->setFocusPolicy(Qt::NoFocus);
    m_pMaximizeButton->setFocusPolicy(Qt::NoFocus);
    m_pCloseButton->setFocusPolicy(Qt::NoFocus);

    m_pTitleLabel->setObjectName("titleLabel");
    m_pMinimizeButton->setObjectName("minimizeButton");
    m_pMaximizeButton->setObjectName("maximizeButton");
    m_pCloseButton->setObjectName("closeButton");

    m_pMinimizeButton->setCheckable(true);
    m_pMaximizeButton->setCheckable(true);
    m_pCloseButton->setCheckable(true);

    m_pMinimizeButton->setToolTip("Minimize");
    m_pMaximizeButton->setToolTip("Maximize");
    m_pCloseButton->setToolTip("Close");

    m_pCustomWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *pLayout = new QHBoxLayout(this);
    pLayout->addWidget(m_pIconLabel);
    pLayout->addSpacing(5);
    pLayout->addWidget(m_pTitleLabel);
    pLayout->addWidget(m_pCustomWidget);
    pLayout->addWidget(m_pMinimizeButton);
    pLayout->addWidget(m_pMaximizeButton);
    pLayout->addWidget(m_pCloseButton);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(5, 0, 5, 0);

    setLayout(pLayout);

    connect(m_pMinimizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pMaximizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));

    updateMaximize();
}
CustomTitleBar::~CustomTitleBar()
{

}

void CustomTitleBar::setMinimumVisible(bool minimum)
{
    if (!minimum)
        m_pMinimizeButton->hide();
    else
        m_pMaximizeButton->show();
}

void CustomTitleBar::setMaximumVisible(bool maximum)
{
    if (!maximum)
        m_pMaximizeButton->hide();
    else
        m_pMaximizeButton->show();
}


QLabel *CustomTitleBar::titleLabel() const
{
    return m_pTitleLabel;
}

void CustomTitleBar::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (m_pWindow->GetHasMaxFun())
    {
        emit m_pMaximizeButton->clicked();
    }
}

bool CustomTitleBar::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::WindowTitleChange: {
        QWidget *pWidget = qobject_cast<QWidget *>(obj);
        if (pWidget) {
            m_pTitleLabel->setText(pWidget->windowTitle());
            return true;
        }
    }
    case QEvent::WindowIconChange: {
        QWidget *pWidget = qobject_cast<QWidget *>(obj);
        if (pWidget) {
            QIcon icon = pWidget->windowIcon();
            m_pIconLabel->setPixmap(icon.pixmap(m_pIconLabel->size()));
            return true;
        }
    }
    case QEvent::Resize:
        updateMaximize();
        return false;       //将消息继续传递,交给m_pWidget的resizeEvent处理,生成合适的背景图片
    default:
        return QWidget::eventFilter(obj, event);
    }
}

void CustomTitleBar::onClicked()
{
    QPushButton *pButton = qobject_cast<QPushButton *>(sender());
    if (m_pWindow == nullptr) return;
    if (m_pWindow->isTopLevel())
    {
        if (pButton == m_pMinimizeButton)
        {
            m_pWindow->OnBtnMinClicked();      //最小化
        }
        else if (pButton == m_pMaximizeButton)
        {
            if (m_pWindow->IsMaxed())
            {
                m_pWindow->OnBtnRestoreClicked();  //恢复
            }
            else
            {
                m_pWindow->OnBtnMaxClicked();      //最大化
            }
        }
        else if (pButton == m_pCloseButton)
        {
            m_pWindow->OnBtnCloseClicked();        //关闭
        }
        updateMaximize();                       //更新按钮样式
    }
}

void CustomTitleBar::updateMaximize()
{
    if (m_pWindow != nullptr) {
        bool bMaximize = m_pWindow->IsMaxed();
        if (bMaximize) {
            m_pMaximizeButton->setToolTip(tr("Restore"));
            m_pMaximizeButton->setProperty("maximizeProperty", "restore");
        } else {
            m_pMaximizeButton->setToolTip(tr("Maximize"));
            m_pMaximizeButton->setProperty("maximizeProperty", "maximize");
        }
        style()->unpolish(m_pMaximizeButton);
        style()->polish(m_pMaximizeButton);
    }
}
