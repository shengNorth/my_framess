#include "framelesshelper.h"
#include "MuShadowWindow.h"

#include <QApplication>
#include <QDesktopWidget>


/***** CursorPosCalculator *****/
CursorPosCalculator::CursorPosCalculator()
{
    reset();
}

void CursorPosCalculator::reset()
{
    m_bOnEdges = false;
    m_bOnLeftEdge = false;
    m_bOnRightEdge = false;
    m_bOnTopEdge = false;
    m_bOnBottomEdge = false;
    m_bOnTopLeftEdge = false;
    m_bOnBottomLeftEdge = false;
    m_bOnTopRightEdge  = false;
    m_bOnBottomRightEdge = false;
}

void CursorPosCalculator::recalculate(const QPoint &gMousePos, QRect frameRect, int shadowWidth, int borderWidth)
{
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();

    //先移除外圈shadow部分,再计算鼠标位置,改变鼠标样式
    frameRect.adjust(shadowWidth, shadowWidth, -shadowWidth, -shadowWidth);

    int frameX = frameRect.x();
    int frameY = frameRect.y();
    int frameWidth = frameRect.width();
    int frameHeight = frameRect.height();
    
    m_bOnLeftEdge = (globalMouseX >= frameX &&
                     globalMouseX <= frameX + borderWidth );
    
    m_bOnRightEdge = (globalMouseX >= frameX + frameWidth - borderWidth &&
                      globalMouseX <= frameX + frameWidth);
    
    m_bOnTopEdge = (globalMouseY >= frameY &&
                    globalMouseY <= frameY + borderWidth );
    
    m_bOnBottomEdge = (globalMouseY >= frameY + frameHeight - borderWidth &&
                       globalMouseY <= frameY + frameHeight);
    
    m_bOnTopLeftEdge = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;
    
    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;
}

/***** WidgetData *****/
WidgetData::WidgetData(FramelessHelper *pHelper, QWidget *pTopLevelWidget)
{
    m_pHelper = pHelper;
    m_pWidget = static_cast<MuShadowWindow<QWidget>*>(pTopLevelWidget);
    m_bLeftButtonPressed = false;
    m_bLeftButtonTitlePressed = false;

    m_pWidget->setMouseTracking(true);
    m_pWidget->setAttribute(Qt::WA_Hover, true);
}

WidgetData::~WidgetData()
{
    m_pWidget->setMouseTracking(false);
    m_pWidget->setAttribute(Qt::WA_Hover, false);
}

QWidget *WidgetData::widget()
{
    return m_pWidget;
}

void WidgetData::handleWidgetEvent(QEvent *event)
{
    switch (event->type()) {
    default:
        break;
    case QEvent::MouseButtonPress:
        handleMousePressEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseReleaseEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMoveEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::Leave:
        handleLeaveEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::HoverMove:
        handleHoverMoveEvent(static_cast<QHoverEvent *>(event));
        break;
    }
}


void WidgetData::updateCursorShape(const QPoint &gMousePos)
{
    if (m_pWidget->IsMaxed()) {
        m_pWidget->unsetCursor();
        return;
    }

    int shadowWidth = m_pWidget->GetShadowWidth();
    int borderWidth = m_pWidget->GetBorderWidth();
    m_pressedMousePos.recalculate(gMousePos, m_pWidget->frameGeometry(), shadowWidth, borderWidth);
    
    if(m_pressedMousePos.m_bOnTopLeftEdge || m_pressedMousePos.m_bOnBottomRightEdge) {
        m_pWidget->setCursor( Qt::SizeFDiagCursor );
    } else if(m_pressedMousePos.m_bOnTopRightEdge || m_pressedMousePos.m_bOnBottomLeftEdge) {
        m_pWidget->setCursor( Qt::SizeBDiagCursor );
    } else if(m_pressedMousePos.m_bOnLeftEdge || m_pressedMousePos.m_bOnRightEdge) {
        m_pWidget->setCursor( Qt::SizeHorCursor );
    } else if(m_pressedMousePos.m_bOnTopEdge || m_pressedMousePos.m_bOnBottomEdge) {
        m_pWidget->setCursor( Qt::SizeVerCursor );
    } else{
        m_pWidget->unsetCursor();
    }
}

void WidgetData::resizeWidget(const QPoint &pos)
{
    QRect origRect = m_resizeDlg;
    qDebug() << m_resizeDlg;
    int shadowWidget = m_pWidget->GetShadowWidth();

        int left = origRect.left();
        int top = origRect.top();
        int right = origRect.right();
        int bottom = origRect.bottom();
        origRect.getCoords(&left, &top, &right, &bottom);

        int minWidth = 200;
        int minHeight = 100;

        if (m_pressedMousePos.m_bOnTopLeftEdge) {
            left = pos.x() - shadowWidget;
            top = pos.y() - shadowWidget;
        } else if (m_pressedMousePos.m_bOnBottomLeftEdge) {
            left = pos.x() - shadowWidget;
            bottom = pos.y() + shadowWidget;
        } else if (m_pressedMousePos.m_bOnTopRightEdge) {
            right = pos.x() + shadowWidget;
            top = pos.y() - shadowWidget;
        } else if (m_pressedMousePos.m_bOnBottomRightEdge) {
            right = pos.x() + shadowWidget;
            bottom = pos.y() + shadowWidget;
        } else if (m_pressedMousePos.m_bOnLeftEdge) {
            left = pos.x() - shadowWidget;
        } else if (m_pressedMousePos.m_bOnRightEdge) {
            right = pos.x() + shadowWidget;
        } else if (m_pressedMousePos.m_bOnTopEdge) {
            top = pos.y() - shadowWidget;
        } else if (m_pressedMousePos.m_bOnBottomEdge) {
            bottom = pos.y() + shadowWidget;
        }

        QRect newRect(QPoint(left, top), QPoint(right, bottom));

        if (newRect.isValid())
        {
            if (newRect.width() < minWidth) {
                if (left != origRect.left())
                    newRect.setLeft(origRect.right() - minWidth);
                else
                    newRect.setRight(origRect.left() + minWidth);
            }
            if (newRect.height() < minHeight) {
                if (top != origRect.top())
                    newRect.setTop(origRect.bottom() - minHeight);
                else
                    newRect.setBottom(origRect.top() + minHeight);
            }
            m_pWidget->setGeometry(newRect);
        }
}

void WidgetData::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = true;
        int shadowWidth = m_pWidget->GetShadowWidth();
        int titleHeight = m_pWidget->GetTitleHeight();

        m_bLeftButtonTitlePressed = event->pos().y() > shadowWidth && event->pos().y() < shadowWidth + titleHeight;
        if (m_bLeftButtonTitlePressed)
        {
            m_startMovePos = event->globalPos();        //记录按下标题栏时候鼠标的位置
        }
        if (m_pressedMousePos.m_bOnEdges)
        {
            m_resizeDlg = m_pWidget->frameGeometry();
        }
    }
}

void WidgetData::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed = false;
        m_bLeftButtonTitlePressed = false;
        m_pressedMousePos.reset();
    }
}

void WidgetData::handleMouseMoveEvent(QMouseEvent *event)
{
    if (m_bLeftButtonPressed) {
        if (m_pHelper->m_bWidgetResizable && m_pressedMousePos.m_bOnEdges) {
            resizeWidget(event->globalPos());
        } else if (m_pHelper->m_bWidgetMovable && m_bLeftButtonTitlePressed) {

            QPoint moveOffset = event->globalPos() - m_startMovePos;

            if (m_pWidget->IsMaxed()
                    && (moveOffset.x() != 0 || moveOffset.y() != 0))
            {
                m_pWidget->OnBtnRestoreClicked();
                QRect wndRect = m_pWidget->frameGeometry();//窗口大小
                QRect screenRect = QApplication::desktop()->availableGeometry(
                            QApplication::desktop()->screenNumber(m_pWidget));//程序所在屏幕可用大小

                int xPos = event->globalPos().x() - wndRect.width() / 2;
                if (xPos < screenRect.left())
                {
                    //超出左边界
                    xPos = screenRect.left();
                }
                else if (xPos + wndRect.width() > screenRect.right())
                {
                    //超出右边界
                    xPos = screenRect.right() - wndRect.width();
                }
                m_pWidget->move(xPos, 0);
            }

            {
                //移动窗口
                QPoint widgetPos = m_pWidget->pos();
                m_startMovePos = event->globalPos();
                m_pWidget->move(widgetPos.x() + moveOffset.x(), widgetPos.y() + moveOffset.y());
            }
        }
    } else if (m_pHelper->m_bWidgetResizable) {
        updateCursorShape(event->globalPos());
    }
}

void WidgetData::handleLeaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    if (!m_bLeftButtonPressed) {
        m_pWidget->unsetCursor();
    }
}

void WidgetData::handleHoverMoveEvent(QHoverEvent *event)
{
    if (m_pHelper->m_bWidgetResizable) {
        updateCursorShape(m_pWidget->mapToGlobal(event->pos()));
    }
}

/*****FramelessHelper*****/
FramelessHelper::FramelessHelper(QObject *parent)
    : QObject(parent)
{
    m_bWidgetMovable = true;
    m_bWidgetResizable = true;
}

FramelessHelper::~FramelessHelper()
{
    if (m_widgetData != nullptr)
    {
        delete m_widgetData;
    }
}

bool FramelessHelper::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::HoverMove || event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::Leave) {
        if (m_widgetData) {
            m_widgetData->handleWidgetEvent(event);
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void FramelessHelper::activateOn(QWidget *topLevelWidget)
{
    if (m_widgetData == nullptr)
    {
        m_widgetData = new WidgetData(this, topLevelWidget);
        //为需要控制的Widget安装事件过滤器
        topLevelWidget->installEventFilter(this);
    }
}

void FramelessHelper::setWidgetMovable(bool movable)
{
    m_bWidgetMovable = movable;
}

void FramelessHelper::setWidgetResizable(bool resizable)
{
    m_bWidgetResizable = resizable;
}

bool FramelessHelper::widgetMovable()
{
    return m_bWidgetMovable;
}

bool FramelessHelper::widgetResizable()
{
    return m_bWidgetResizable;
}
