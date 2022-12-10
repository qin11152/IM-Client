#pragma once
/*****************************************************************//**
 * \file   NotifyLabel.h
 * \brief  ֪ͨlabel��������ʾ֪ͨ��Ϣ����ʱ��ʧ
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
     * brief�������趨��ʾ��λ�ã��������������ʾ��Ĭ�ϵ�λ��.
     * 
     * \param x
     * \param y
     * \param w
     * \param h
     * \return 
     */
    void setPositions(int x, int y, int w, int h);

    /**
     * brief:�����趨Ҫ��ʾ����Ϣ.
     * 
     * \param notifyInfo
     */
    void setNitifyText(const char* notifyInfo);

    /*
     * brief����ʼ��ʾ.
     *
     */
    void showNotify();

    /**
     * brief��ֹͣ��ʾ.
     * 
     */
    void stopNotify();

    /**
     * brief���趨��ʾ���.
     * 
     * \param interval
     */
    void setShowTime(const int interval);

    /**
     * brief���鿴�Ƿ�����ʹ��.
     * 
     * \return 
     */
    bool getRunningState()const { return m_bIsRunning; }

    /**
     * brief���ж�label�ı��Ƿ���Ҫ�������ʾ.
     * 
     */
    void upateLabelRollingState();

private slots:
    /**
     * brief���趨����ʾʱ�䵽��.
     */
    void onSignalTimeout();
    
    /**
     * brief������ƶ�ʱ��.
     * 
     */
    void onSignalScrollTimeout();

private:
    QTimer* m_timer{ nullptr };     //��ʱ��ʧ��timer
    QTimer* m_scrollTimer{ nullptr };//������ö�ʱ��
    QRect m_rect;                   //������¼Ҫ��ʾ��λ��
    int m_iTimerInterval{ 5 * 1000 };//Ĭ�϶�ʱʱ��
    int m_iScrollDisplayPos{ 0 };   //��ǰ�������ʾ���ַ�λ��
    bool m_bIsRunning{ false };     //�Ƿ�������
    QString m_strNotifyInfo;
};
