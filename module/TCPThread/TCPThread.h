#pragma once


#include "module/stdafx.h"
#include "module/TCPThread/MyTCPSocket.h"

#include "boost/noncopyable.hpp"
#include <boost/serialization/singleton.hpp>

#include <QThread>
#include <QTimer>

struct LengthInfo
{
    int length;
    LengthInfo(int l) :length(l) {}
};

class ChatWidget;

class TCPThread  : public QThread, public boost::noncopyable, public boost::serialization::singleton<TCPThread>
{
    Q_OBJECT

public:
    TCPThread(QObject *parent=nullptr);
    ~TCPThread();

    //反初始化
    void unInit();
    //重写run函数
    void run()override;

public slots:
    //调用该函数像服务器发送消息
    void sendMessage(std::string message);
    //调用该函数发送图片消息
    void sendImageMsg(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);

signals:
    //收到注册结果的消息
    void signalRecvRegisterMessage(const QString& msg);
    //收到登录结果消息
    void signalRecvLoginResultMessage(const QString& msg);
    //收到朋友聊天消息
    void signalRecvSingleChatMessage(const QString& msg);
    //收到好友列表消息
    void signalRecvFriendListMessage(const QString& msg);
    //服务端发送来的好友请求
    void signalNewFriendRequest(const QString& msg);
    //服务端发来的添加好友后的通知
    void signalBecomeFriendNotify(const QString& msg);
    //与服务器连接失败信号
    void signalConnectFailed();
    //超过一定时间还没收到服务器的心跳包
    void signalTimeoutNoHeartPackage();
    //某个好友头像更新了的信号
    void signalProfileImageChanged(const QString& id,const QString& path);
    //添加好友时服务器推送过来的头像信息
    void signalAddFriendProfileImage(const QString& id, const QString& path);

    /////为了能够在子线程中发送接受消息，需要再封装一层
    //发送消息信号
    void signalSendMsg(const std::string& msg);
    //发送图片消息信号
    void signalSendImageMsg(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);
    //连接的信号
    void signalConnect();

private slots:
    //超过时间还没收到回复
    void onSignalTimeoutNoHeartPackage();
    //到时间就给服务端发一个心跳包
    void onSignalTimeoutSendHeartPackage();
    //收到消息的处理
    void onSignalRecvMessage();
    //收到消息的处理分派
    void onHandleMessage(const std::string& recvMessage);
    //收到连接的结果
    void onConnectResult(bool bResult);

private:
    MyTCPSocket* m_ptrTcpSocket{ nullptr };      //socket指针
    int m_iPort{ 9999 };                        //服务器端口
    QString m_strIp{ "43.142.158.231" };         //服务器ip
    bool m_bIsConnected{ false };                   //是否第一次发送消息
    QTimer* m_ptrTimerKeepAlive{ nullptr };
    QTimer* m_ptrTimerRecvHeartPackage{ nullptr };
    char m_msgBuffer[kMsgBufferLength]{ 0 };                      //用于存储消息的缓冲区
    int m_endPosOfBuffer = 0;                                     //消息缓冲区的尾部

    //处理服务器传来的图片消息
    std::unordered_map<std::string, std::string> m_mapImageUUIDAndBase64;
    std::unordered_map<std::string, int> m_mapImageUUIDAndSegment;

    //与服务器建立连接的函数
    void connectHost();
    //初始化槽函数连接
    void initConnect();
    //初始化函数
    void init();
    //与服务器断开连接
    void disConnect();
};

