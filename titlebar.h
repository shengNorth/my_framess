/**
 * 自定义无边框窗体、对话框和提示框
 *
 * MuTitleBar.h
 * 自定义窗体的标题栏。
 *
 * FlyWM_
 * GitHub: https://github.com/FlyWM
 * CSDN: https://blog.csdn.net/a844651990
 *
 */

#ifndef MUTITLEBAR_H
#define MUTITLEBAR_H

#include <QWidget>

class QLabel;
class QPushButton;

template<typename T>
class MuShadowWindow;

class  MuTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit MuTitleBar(QWidget *parent, QWidget *window);
    ~MuTitleBar();

    void setMinimumVisible(bool minimum);
    void setMaximumVisible(bool maximum);

    void setTitleHeight(int height);

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

signals:
    void ShowMaximized();
    void ShowNormal();
    void HeightChanged(int height);

private:
    /**
     * @brief updateMaximize update the button status
     */
    void updateMaximize();

private:
    QLabel *m_pIconLabel;
    QLabel *m_pTitleLabel;
    QPushButton *m_pMinimizeButton;
    QPushButton *m_pMaximizeButton;
    QPushButton *m_pCloseButton;
    QWidget*    m_pCustomWidget; // 图标，标题，最大最小关闭按钮之外，自定义添加的内容

    MuShadowWindow<QWidget>*    m_pWindow = nullptr;
};

#endif // MUTITLEBAR_H
