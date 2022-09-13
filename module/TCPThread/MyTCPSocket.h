#pragma once

#include "boost/noncopyable.hpp"
#include <QTCPSocket>

class MyTCPSocket : public QTcpSocket, public boost::noncopyable
{
    Q_OBJECT

public:
    MyTCPSocket(QObject* parent = nullptr);
    ~MyTCPSocket();

public:
    //������Ϣ�ۺ����������װһ�㣬�������ֶ��߳����⣨QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread��
    void sendMsg(std::string msg);
    //����ͼƬ��Ϣ�ۺ���
    void sendImageMsg(QString& strBase64Image, const QString& ImageName);
    
};
