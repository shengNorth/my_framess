#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>

class QLabel;
class QPushButton;

template<typename T>
class ShadowWindow;

class  CustomTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit CustomTitleBar(QWidget *parent, QWidget *window);
    ~CustomTitleBar();

    void setMinimumVisible(bool minimum);
    void setMaximumVisible(bool maximum);

    QWidget *customWidget() const;
    QPushButton *minimizeButton() const;
    QPushButton *maximizeButton() const;
    QPushButton *closeButton() const;
    QLabel *titleLabel() const;

protected:
    virtual void paintEvent(QPaintEvent *e);

    /**
     * @brief mouseDoubleClickEvent 双击标题栏进行界面的最大化/还原
     * @param event QMouseEvent *
     */
    virtual void mouseDoubleClickEvent(QMouseEvent *event);   

    /**
     * @brief eventFilter 设置界面标题与图标
     * @param obj
     * @param event
     * @return
     *  bool
     */
    virtual bool eventFilter(QObject *obj, QEvent *event);

private slots:
    /**
     * @brief onClicked 进行最小化、最大化/还原、关闭操作
     */
    void onClicked();

private:
    /**
     * @brief updateMaximize update the button status
     */
    void updateMaximize();

private:
    QLabel*         m_pIconLabel;
    QLabel*         m_pTitleLabel;
    QPushButton*    m_pMinimizeButton;
    QPushButton*    m_pMaximizeButton;
    QPushButton*    m_pCloseButton;
    QWidget*        m_pCustomWidget; // 图标，标题，最大最小关闭按钮之外，自定义添加的内容

    ShadowWindow<QWidget>*    m_pWindow = nullptr;
};

#endif // TITLEBAR_H
