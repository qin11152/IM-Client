#pragma once

#include "boost/noncopyable.hpp"
#include <QTCPSocket>

class MyTCPSocket : public QTcpSocket, public boost::noncopyable
{
    Q_OBJECT

public:
    MyTCPSocket(QObject* parent = nullptr);
    ~MyTCPSocket();

signals:
    void signalConnectResult(bool bResult);

public slots:
    //������Ϣ�ۺ����������װһ�㣬�������ֶ��߳����⣨QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread��
    void sendMsg(const std::string& msg);
    //����ͼƬ��Ϣ�ۺ���
    void sendImageMsg(const QString& strBase64Image, const QString& ImageName, const QString& suffix);
    //����������
    void connectHost();

private:
    void initConnect();
    
};
