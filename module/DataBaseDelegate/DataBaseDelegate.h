#pragma once

#include "../module/MyDefine.h"
#include <QObject>
#include <QSqlDatabase>
#include <memory>
#include <mutex>
#include <map>
#include <vector>


class DataBaseDelegate : public QObject
{
    Q_OBJECT
    using SingletonPtr = std::shared_ptr<DataBaseDelegate>;

public:
    //单例模式获取对象唯一方式
    static SingletonPtr Instance();
    ~DataBaseDelegate();
    DataBaseDelegate(DataBaseDelegate& l) = delete;
    DataBaseDelegate& operator=(DataBaseDelegate& l) = delete;
    void SetUserId(QString id);
    int GetChatRecordCountFromDB(QString id);
    //创建与某个用户的聊天记录表
    bool createUserChatTable(const QString& userid);
    //创建上次聊天表
    bool createLastChatListTable();
    //创建好友请求的表
    bool createFriendRequestTable();
    //创建lastchat备份表
    bool createLastChatBackUp();
    //向lastchat中插入记录
    bool insertLastChat(QString id, QString pos);
    //只有id
    bool insertLastChat(QString id);
    //插入聊天记录
    bool insertChatRecoed(int TotalCount,const QString& userid, const QString& message, const QString& time,bool isSelf,const QString& name);
    //插入添加好友请求
    bool insertAddFriendRequest(const QString& id,const QString& name,const QString& verifyMsg);
    //判断表是否存在
    bool isTableExist(const QString& tableNmae);
    //从数据库中查找出上次关闭时的聊天列表
    bool queryLastChatListFromDB(std::vector<MyLastChatFriendInfo>& m_tmpVec);
    //根据id查找聊天记录
    bool queryChatRecordAcodIdFromDB(QString id, std::vector<MyChatMessageInfo>& chatMessage,int queryCount,int beginPos);
    bool QueryInitialAcordIdFromDB(QString id, QString& str);
    //获取好友请求信息，包括已添加和未添加
    bool queryAddFriendInfoFromDB(QString id,std::vector<MyAddFriendInfo>&addFriendInfo);
    //根据name去查找好友请求对应的id
    bool queryFriendRequestAcordName(QString name,QString& id);
    //根据name把响应的好友请求置为true
    bool updateFriendRequestStateAcordName(QString name);
    //删除过期的好友请求,这里采用30天为期限,无论是否同意都删除
    bool deleteExpiredFriendRequest();
    //删除lastchat中的内容
    bool deleteLastChatInfo();
    //copylastchat表到备份中，然后更新lastchat
    bool copyLastChatToBackUps();

private:
    DataBaseDelegate(QObject* parent=nullptr);
    static std::mutex m_mutex;
    static SingletonPtr m_SingletonPtr;
    QSqlDatabase m_dataBase;
    //数据库也应由id来指定，不同用户登录建立不同数据库
    QString m_strUserId{ "" };

    //初始化连接，创建库，表等
    void init();
};


