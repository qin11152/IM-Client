/************************************************************************/
/* 聊天界面逻辑部门管理类，相关逻辑操作均定义在此
/************************************************************************/
#pragma once

#include "module/stdafx.h"
#include "module/ThreadPool/ThreadPool.h"
#include "ui/MyChatMessageQuickWid/MyChatMessageQuickWid.h"
#include "module/LastChatInfoUpdateThread/DatabaseOperateThread.h"

#include <QObject>

namespace module
{
    class ChatWidgetManager : public QObject
    {
        Q_OBJECT

    public:
        using SingletonPtr = std::shared_ptr<ChatWidgetManager>;
        //单例模式，唯一获取其指针的方法，使用了智能指针，自动内存管理
        static SingletonPtr Instance();
        //无需拷贝构造和赋值构造函数
        ChatWidgetManager(const ChatWidgetManager&) = delete;
        ChatWidgetManager& operator=(const ChatWidgetManager&) = delete;
        ~ChatWidgetManager()override;

        //传入id
        void setUserId(QString strId);
        //传入名字
        void setUserName(QString& name);
        //设置qml根对象指针，以便调用qml中的函数
        void setQMLRootPtr(QObject* AddFriendQMLRoot, QObject* FriendListQMLRoot, QObject* LastChatQMLRoot);
        //初始化数据库操作子线程
        void initDBOperateThread();
        //初始化数据库操作子线程连接
        void initDBThreadConnect();
        //设置子线程中最新的lastchat列表
        void setLastChatList(std::vector<std::pair<QString, bool>>& modelOrder)const;

        /**
         * brief:初始化每个账号所需要的文件夹和文件，如果不存在就创建.
         *
         */
        void initDirAndFile();

        //从服务端获取当前用户的好友列表
        void getFriendList();
        //通知服务端客户端上线
        void notifyServerOnline();
        //获取上次聊天的好友列表
        void getLastChatListFromDB(std::vector <MyLastChatFriendInfo>& vecLastChatFriend);
        //将获取到的好有时间戳和数据库中的进行对比
        void compareImageTimestap(std::vector<MyFriendInfoWithFirstC> vecFriendInfo);
        //将vec里的好友id和头像时间戳数据库进行一个比对,数据库没有就获取下
        void compareImageTimeStamp(std::vector<std::string> vecId);

        //初始化的时候获取聊天记录，10条或者小于10条
        std::vector<MyChatMessageInfo> getChatMessageAcordIdAtInit(QString strId);
        //收到好友列表消息后

        void handleSingleChatMessage(const QString& msg, const QString& id, const std::string timeStamp, const wechat::MyChatMessageQuickWid* chatWidgetPtr);

    public slots:

        //服务器传来的好友列表信息
        void onSignalRecvFriendList(const QString& friendList, std::unordered_map<QString, int>& mapUserInfo, std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC);
        //收到qml页面同意添加好友的请求
        void onSignalAgreeAddFriend(const QString& friendId);
        //qml页面发送添加请求
        void onSignalRequestAddFriend(QString friendId, QString verifyMsg);
        //服务器通知成为了好友
        void onSignalBecomeFriend(const QString& msg);
        //服务器通知有人加好友
        void onSignalNewFriendRequest(const QString& msg);
        //要求修改数据库中lastchat的内容
        void onSignalUpdateLastChat();
        //要求从lastchat的qml页面中获取当前的顺序
        void onSignalGetModelOrder(std::vector<std::pair<QString, bool>>& modelOrder);

        //群聊相关
        void onSignalStartGroupChatReply(const QString& msg);

    signals:
        void signalGetFriendListFinished();
        //有好友同意请求了,发送给chatwidget界面，让他更新界面
        void signalBecomeFriend(const MyFriendInfoWithFirstC& friendInfo);
        //通知lastchat界面增加一个群聊
        void signalAddGroupChat2LastChat(const MyFriendInfoWithFirstC& friendInfo);

    private:
        ChatWidgetManager(QObject* parent = nullptr);
        static std::mutex m_mutex;                  //锁，保证线程安全
        static SingletonPtr m_SingletonPtr;         //该类的智能指针
        QString m_strUserId{ "" };                  //自己的id
        QString m_strUserName{ "" };                //自己的昵称
        ThreadPool* m_ptrThreadPool{ nullptr };     //线程池

        QObject* m_ptrLastChatQMLRoot{ nullptr }; //上次聊天qml的根对象
        QObject* m_ptrFriendListQMLRoot{ nullptr };//好友列表qml的根对象
        QObject* m_ptrAddFriendQMLRoot{ nullptr };  //添加好友qml界面的根对象

        DatabaseOperateThread* m_ptrDBOperateThread{ nullptr };  //用于操作lastchat数据库的子线程

        void initConnect();         //初始化信号槽连接
    };
}
