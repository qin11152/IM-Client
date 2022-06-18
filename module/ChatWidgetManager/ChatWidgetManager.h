/************************************************************************/
/* 聊天界面逻辑部门管理类，相关逻辑操作均定义在此                                                                   
/************************************************************************/
#pragma once

#include "../MyDefine.h"
#include "protocol/GetFriendListJsonData/GetFriendListJsonData.h"
#include <QObject>
#include <mutex>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>

class ChatWidgetManager : public QObject
{
    Q_OBJECT

public:
    using SingletonPtr = std::shared_ptr<ChatWidgetManager>;
    //单例模式，唯一获取其指针的方法，使用了智能指针，自动内存管理
    static SingletonPtr Instance();
    //传入id
    void setUserId(QString strId);
    //传入名字
    void setUserName(QString& name);
    //无需拷贝构造和赋值构造函数
    ChatWidgetManager(const ChatWidgetManager&) = delete;
    ChatWidgetManager& operator=(const ChatWidgetManager&) = delete;
    ~ChatWidgetManager();

    //设置qml根对象指针，以便调用qml中的函数
    void setQMLRootPtr(QObject* AddFriendQMLRoot, QObject* FriendListQMLRoot, QObject* LastChatQMLRoot);

    //从服务端获取当前用户的好友列表
    void getFriendList();
    //通知服务端客户端上线
    void notifyServerOnline();
    //获取上次聊天的好友列表
    void getLastChatListFromDB(std::vector<QString>& vecLastChatFriend);

    //初始化的时候获取聊天记录，10条或者小于10条
    std::vector<MyChatMessageInfo> getChatMessageAcordIdAtInit(QString strId);
    //收到好友列表消息后

public slots:

    //服务端返回来好友列表
    void onSignalRecvFriendList(const QString& friendList, std::map<QString, int>& mapUserInfo, std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC);
    //收到qml页面同意添加好友的请求
    void onSignalAgreeAddFriend(QString friendName);
    //qml页面发送添加请求
    void onSignalRequestAddFriend(QString friendId, QString verifyMsg);
    //服务器通知成为了好友
    void onSignalBecomeFriend(const QString& msg);
    //服务器通知有人加好友
    void onSignalNewFriendRequest(const QString& msg);


signals:
    void signalGetFriendListFinished();
    //有好友同意请求了
    void signalAddFriendToLastChat(const MyFriendInfoWithFirstC& friendInfo);

private:
    //初始化连接
    void initConnect();

private:
    ChatWidgetManager(QObject* parent = nullptr);
    static std::mutex m_mutex;                  //锁，保证线程安全
    static SingletonPtr m_SingletonPtr;         //该类的智能指针
    QString m_strUserId{ "" };                  //自己的id
    QString m_strUserName{ "" };                //自己的昵称

    QObject* m_ptrLastChatQMLRoot{ nullptr }; //上次聊天qml的根对象
    QObject* m_ptrFriendListQMLRoot{ nullptr };//好友列表qml的根对象
    QObject* m_ptrAddFriendQMLRoot{ nullptr };  //添加好友qml界面的根对象
};
