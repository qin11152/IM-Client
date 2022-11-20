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
    //发送消息槽函数，必须封装一层，否则会出现多线程问题（QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread）
    void sendMsg(const std::string& msg);
    //发送图片消息槽函数
    void sendImageMsg(const QString& strBase64Image, const QString& ImageName, const QString& suffix);
    //与主机连接
    void connectHost();

private:
    void initConnect();
    
};
