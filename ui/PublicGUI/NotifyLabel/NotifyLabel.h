#pragma once
/*****************************************************************//**
 * \file   NotifyLabel.h
 * \brief  通知label，用于显示通知信息，定时消失
 * 
 * \author Administrator
 * \date   December 2022
 *********************************************************************/

#include <QLabel>
#include <QTimer>

class NotifyLabel  : public QLabel
{
    Q_OBJECT

public:
    NotifyLabel(QWidget* parent = nullptr);
    ~NotifyLabel();
    /**
     * brief：用来设定显示的位置，如果不设置则显示在默认的位置.
     * 
     * \param x
     * \param y
     * \param w
     * \param h
     * \return 
     */
    void setPositions(int x, int y, int w, int h);

    /**
     * brief:用来设定要显示的消息.
     * 
     * \param notifyInfo
     */
    void setNitifyText(const char* notifyInfo);

    /*
     * brief：开始显示.
     *
     */
    void showNotify();

    /**
     * brief：停止显示.
     * 
     */
    void stopNotify();

    /**
     * brief：设定显示多久.
     * 
     * \param interval
     */
    void setShowTime(const int interval);

    /**
     * brief：查看是否正在使用.
     * 
     * \return 
     */
    bool getRunningState()const { return m_bIsRunning; }

    /**
     * brief：判断label文本是否需要跑马灯显示.
     * 
     */
    void upateLabelRollingState();

private slots:
    /**
     * brief：设定的显示时间到了.
     */
    void onSignalTimeout();
    
    /**
     * brief：跑马灯定时器.
     * 
     */
    void onSignalScrollTimeout();

private:
    QTimer* m_timer{ nullptr };     //定时消失的timer
    QTimer* m_scrollTimer{ nullptr };//跑马灯用定时器
    QRect m_rect;                   //用来记录要显示的位置
    int m_iTimerInterval{ 5 * 1000 };//默认定时时间
    int m_iScrollDisplayPos{ 0 };   //当前跑马灯显示的字符位置
    bool m_bIsRunning{ false };     //是否在运行
    QString m_strNotifyInfo;
};
