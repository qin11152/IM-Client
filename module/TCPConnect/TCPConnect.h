#pragma once

#include <QObject>
#include <QTcpSocket>
#include <mutex>
#include <memory>
#include <QTimer>
#include "../module/MyDefine.h"

namespace
{
    constexpr int kHeartTimeoutTime = 300 * 1000;
    constexpr int kKeepAlivePackageTime = 299 * 1000;
    constexpr int kMsgBufferLength = 1024 * 10;
}

struct LengthInfo
{
    int length;
    LengthInfo(int l) :length(l) {}
};

class TCPConnect : public QObject
{
    Q_OBJECT

public:
    using SingletonPtr = std::shared_ptr<TCPConnect>;
    //单例模式，唯一获取其指针的方法，使用了智能指针，自动内存管理
    static SingletonPtr Instance();
    //调用该函数像服务器发送消息
    void sendMessage(std::string& message);
    void sendLength(LengthInfo& l, int length);
    ~TCPConnect();
    //无需拷贝构造和赋值构造函数
    TCPConnect(const TCPConnect&) = delete;
    TCPConnect& operator=(const TCPConnect&) = delete;
signals:
    //收到注册结果的消息
    void signalRecvRegisterMessage(const QString& msg);
    //收到登录结果消息
    void signalRecvLoginResultMessage(const QString& msg);
    //收到朋友聊天消息
    void signalRecvSingleChatMessage(const QString& msg);
    //与服务器连接失败信号
    void signalConnectFailed();
    //超过一定时间还没收到服务器的心跳包
    void signalTimeoutNoHeartPackage();

private slots:
    //超过时间还没收到回复
    void onSignalTimeoutNoHeartPackage();
    //到时间就给服务端发一个心跳包
    void onSignalTimeoutSendHeartPackage();
    //收到消息的处理
    void onSignalRecvMessage();
    //收到消息的处理分派
    void onHandleMessage(const std::string& recvMessage);

private:
    QTcpSocket* m_ptrTcpSocket{ nullptr };      //socket指针
    static std::mutex m_mutex;                  //锁，保证线程安全
    static SingletonPtr m_SingletonPtr;         //该类的智能指针
    int m_iPort{ 9999 };                        //服务器端口
    QString m_strIp{ "39.104.207.91" };         //服务器ip
    bool m_bIsConnected{ false };                   //是否第一次发送消息
    QTimer* m_ptrTimerKeepAlive{ nullptr };
    QTimer* m_ptrTimerRecvHeartPackage{ nullptr };
    char m_msgBuffer[kMsgBufferLength]{ 0 };                      //用于存储消息的缓冲区
    int m_endPosOfBuffer = 0;                                     //消息缓冲区的尾部

    //与服务器建立连接的函数
    void connectHost();
    //初始化槽函数连接
    void initConnect();
    //初始化函数
    void init();
    //构造函数，私有
    TCPConnect(QObject* parent=nullptr);
};


