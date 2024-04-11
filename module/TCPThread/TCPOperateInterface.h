#pragma once
#include "module/stdafx.h"
#include "boost/serialization/singleton.hpp"

#include <QObject>
#include <qtcpsocket.h>
#include <qtimer.h>
#include <qthread.h>

namespace module
{
    class TCPOperateInterface;
    class TCPOperate : public QObject
    {
        Q_OBJECT

    public:
        TCPOperate(TCPOperateInterface* ptr, QObject* parent);
        ~TCPOperate();

    signals:
        //调用该函数像服务器发送消息
        void sendMessage(std::string message);

        //超过一定时间还没收到服务器的心跳包
        void signalTimeoutNoHeartPackage();

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
        //某个好友头像更新了的信号
        void signalProfileImageChanged(const QString& id, const QString& path);
        //添加好友时服务器推送过来的头像信息
        void signalAddFriendProfileImage(const QString& id, const QString& timeStamp, const QString& path);
        //群聊相关
        void signalStartGroupChatReply(const QString& msg);

        /////为了能够在子线程中发送接受消息，需要再封装一层
        //发送消息信号
        void signalSendMsg(const std::string& msg);
        //发送图片消息信号
        void signalSendImageMsg(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);

    public slots:
        /**
         * brief:tcp发送接收线程开始工作.
         *
         */
        void init();

        void unInit();

        void sendMessageInternal(std::string message);

        void sendImageInternel(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);

        void onSignalTimeoutSendHeartPackage();

        void onSignalTimeoutNoHeartPackage();

        void onSignalRecvMessage();

        void connectHost();

        void disconnectHost();

    private:
        void onHandleMessage(const std::string& recvMessage);

    private:
        QTcpSocket* m_ptrTCPSocket{ nullptr };
        TCPOperateInterface* m_ptrExternal{ nullptr };

        bool m_bConnectResult{ false };
        int m_iPort{ 19999 };                        //服务器端口
        QString m_strIp{ "43.142.158.231" };         //服务器ip
        QTimer* m_timerKeepAlive{ nullptr };
        //QTimer m_timerRecvHeartPackage;
        char m_msgBuffer[kMsgBufferLength]{ 0 };                      //用于存储消息的缓冲区
        size_t m_endPosOfBuffer = 0;                                     //消息缓冲区的尾部
        //处理服务器传来的图片消息
        std::unordered_map<std::string, std::string> m_mapImageUUIDAndBase64;
        std::unordered_map<std::string, int> m_mapImageUUIDAndSegment;
    };

    class TCPOperateInterface : public QObject, public boost::serialization::singleton<TCPOperateInterface>
    {
        Q_OBJECT
    public:
        TCPOperateInterface();
        ~TCPOperateInterface();

        void sendMessageExternalInterface(std::string message);

        //调用该函数发送图片消息
        void sendImageMsgExternalInterface(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);

        void disconnect();

    signals:
        void signalSendMessageExternal(std::string message);

        //调用该函数发送图片消息
        void signalSendImageMsgExternal(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);


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
        //某个好友头像更新了的信号
        void signalProfileImageChanged(const QString& id, const QString& path);
        //添加好友时服务器推送过来的头像信息
        void signalAddFriendProfileImage(const QString& id, const QString& timeStamp, const QString& path);
        //群聊相关
        void signalStartGroupChatReply(const QString& msg);

    private:
        void initConnect();

    private:
        QThread m_operateThread;
        TCPOperate* m_ptrTCPOperate{ nullptr };
    };
}
