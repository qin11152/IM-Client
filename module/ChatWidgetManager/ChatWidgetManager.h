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
    //无需拷贝构造和赋值构造函数
    ChatWidgetManager(const ChatWidgetManager&) = delete;
    ChatWidgetManager& operator=(const ChatWidgetManager&) = delete;
    ~ChatWidgetManager();

    //从服务端获取当前用户的好友列表
    void getFriendList();
    //通知服务端客户端上线
    void notifyServerOnline();
    //获取上次聊天的好友列表
    void getLastChatListFromDB();

    //获取好友列表
    std::vector<MyFriendInfoWithFirstC> getMyFriendVec()const { return m_vecFriendInfoWithC; }
    //初始化的时候获取聊天记录，10条或者小于10条
    std::vector<MyChatMessageInfo> getChatMessageAcordIdAtInit(QString strId);
    //获取上次聊天列表
    std::vector<QString> getLastChatList()const { return m_vecLastChatFriend; }
    //根据id获取这个好友的信息
    MyFriendInfoWithFirstC getFriendInfo(QString strId);

signals:
    void signalGetFriendListFinished();

private:
    //初始化连接
    void initConnect();
    //收到好友列表消息后
    void onSignalRecvFriendList(const QString& friendList);

private:
    ChatWidgetManager(QObject* parent = nullptr);
    static std::mutex m_mutex;                  //锁，保证线程安全
    static SingletonPtr m_SingletonPtr;         //该类的智能指针
    QString m_strUserId{ "" };                  //自己的id

    //存储好友信息，带首字母
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;
    std::map<QString,int> m_mapUserInfo;     //存储好友id和该id在vec中对应的位置，以便查找信息
    std::vector<QString> m_vecLastChatFriend;        //上次聊天页面里的好友
};
