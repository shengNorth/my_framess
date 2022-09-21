#ifndef FRAMELESSHELPER_H
#define FRAMELESSHELPER_H

#include <QtGui>
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

class WidgetData;
class FramelessHelper;

template<typename T>
class MuShadowWindow;

class LinuxRubberBand : public QRubberBand
{
public:
    LinuxRubberBand(Shape s, QWidget *p = 0)
        : QRubberBand(s, p) {
        QPalette palette;
        palette.setBrush(QPalette::WindowText, QBrush(Qt::lightGray));
        setPalette(palette);
        repaint();
    }

protected:
    virtual void paintEvent(QPaintEvent *) {
        QStylePainter painter(this);
        QStyleOptionFocusRect option;
        option.initFrom(this);

        QPen pen;
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        pen.setColor(QColor(Qt::red));
        painter.setPen(pen);
        painter.drawControl(QStyle::CE_FocusFrame, option);
    }

};

/*****
 * CursorPosCalculator
 * 计算鼠标是否位于左、上、右、下、左上角、左下角、右上角、右下角
*****/
class CursorPosCalculator
{
public:
    explicit CursorPosCalculator();
    void reset();
    void recalculate(const QPoint &gMousePos, QRect frameRect, int shadowWidth, int borderWidth);

public:
    bool m_bOnEdges = true;
    bool m_bOnLeftEdge = true;
    bool m_bOnRightEdge = true;
    bool m_bOnTopEdge = true;
    bool m_bOnBottomEdge = true;
    bool m_bOnTopLeftEdge = true;
    bool m_bOnBottomLeftEdge = true;
    bool m_bOnTopRightEdge = true;
    bool m_bOnBottomRightEdge = true;
};

/*****
 * WidgetData
 * 更新鼠标样式、移动窗体、缩放窗体
*****/
class WidgetData
{
public:
    explicit WidgetData(FramelessHelper* pHelper, QWidget *pTopLevelWidget);
    ~WidgetData();
    QWidget *widget();
    // 处理鼠标事件-划过、厉害、按下、释放、移动
    void handleWidgetEvent(QEvent *event);

private:
    // 更新鼠标样式
    void updateCursorShape(const QPoint &gMousePos);
    // 重置窗体大小
    void resizeWidget(const QPoint &gMousePos);
    // 处理鼠标按下
    void handleMousePressEvent(QMouseEvent *event);
    // 处理鼠标释放
    void handleMouseReleaseEvent(QMouseEvent *event);
    // 处理鼠标移动
    void handleMouseMoveEvent(QMouseEvent *event);
    // 处理鼠标离开
    void handleLeaveEvent(QEvent *event);
    // 处理鼠标进入
    void handleHoverMoveEvent(QHoverEvent *event);

private:
    FramelessHelper*    m_pHelper = nullptr;
    MuShadowWindow<QWidget>* m_pWidget;
    QPoint m_startMovePos;
    QRect m_resizeDlg;
    CursorPosCalculator m_pressedMousePos;
    bool m_bLeftButtonPressed;          //鼠标是否按下
    bool m_bLeftButtonTitlePressed;     //鼠标是否按在标题栏上
};

class FramelessHelper : public QObject
{
    Q_OBJECT

public:
    explicit FramelessHelper(QObject *parent = 0);
    ~FramelessHelper();
    // 激活窗体
    void activateOn(QWidget *topLevelWidget);
    // 设置窗体移动
    void setWidgetMovable(bool movable);
    // 设置窗体缩放
    void setWidgetResizable(bool resizable);

    bool widgetResizable();
    bool widgetMovable();

protected:
    // 事件过滤，进行移动、缩放等
    virtual bool eventFilter(QObject *obj, QEvent *event);

public:
    bool m_bWidgetMovable = true;           //是否可移动
    bool m_bWidgetResizable = true;         //是否可缩放

private:
    WidgetData* m_widgetData = nullptr;
};

#endif // FRAMELESSHELPER_H
