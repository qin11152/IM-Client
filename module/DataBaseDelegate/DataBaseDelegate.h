#pragma once

#include "../module/MyDefine.h"
#include <QObject>
#include <QSqlDatabase>
#include <QThread>
#include <memory>
#include <mutex>
#include <vector>


class DataBaseDelegate : public QObject
{
    Q_OBJECT
    using SingletonPtr = std::shared_ptr<DataBaseDelegate>;

public:
    //单例模式获取对象唯一方式
    static SingletonPtr Instance();
    ~DataBaseDelegate() override;
    DataBaseDelegate(DataBaseDelegate& l) = delete;
    DataBaseDelegate& operator=(DataBaseDelegate& l) = delete;
    //为数据库设置用户的id
    void setUserId(const QString& id);
    //根据id获取聊天记录数量
    int getChatRecordCountFromDB(const QString& id)const;
    //创建与某个用户的聊天记录表
    bool createUserChatTable(const QString& userid)const;
    //创建上次聊天表
    bool createLastChatListTable()const;
    //创建好友请求的表
    bool createFriendRequestTable()const;
    //只有id
    bool insertLastChat(const QString& id)const;
    //插入到上次聊天表中
    bool insertLastChat(const std::vector<QString>& order)const;
    //清空上次聊天表
    bool clearLastChat()const;
    //插入聊天记录
    bool insertChatRecoed(int TotalCount, const QString& userid, const QString& message, const QString& time,
                          bool isSelf, const QString& name)const;
    //插入添加好友请求
    bool insertAddFriendRequest(const QString& id, const QString& name, const QString& verifyMsg)const;
    //判断表是否存在
    bool isTableExist(const QString& tableName)const;
    //从数据库中查找出上次关闭时的聊天列表
    bool queryLastChatListFromDB(std::vector<MyLastChatFriendInfo>& m_tmpVec)const;
    //根据id查找聊天记录
    bool queryChatRecordAcodIdFromDB(QString id, std::vector<MyChatMessageInfo>& chatMessage, int queryCount,
                                     int beginPos)const;
    bool QueryInitialAcordIdFromDB(const QString& id, QString& str)const;
    //获取好友请求信息，包括已添加和未添加
    bool queryAddFriendInfoFromDB(QString id, std::vector<MyAddFriendInfo>& addFriendInfo)const;
    //根据name去查找好友请求对应的id
    bool queryFriendRequestAcordName(const QString& name, QString& id)const;
    //根据name把响应的好友请求置为true
    bool updateFriendRequestStateAcordName(const QString& name)const;
    //删除过期的好友请求,这里采用30天为期限,无论是否同意都删除
    bool deleteExpiredFriendRequest()const;
    //删除lastchat中的内容
    bool deleteLastChatInfo()const;
    //初始化连接，创建库，表等
    void init();
    //断开连接
    void disConnect();
    
private:
    DataBaseDelegate(QObject* parent = nullptr);
    static std::mutex m_mutex;
    static SingletonPtr m_SingletonPtr;
    QSqlDatabase m_dataBase;
    //数据库也应由id来指定，不同用户登录建立不同数据库
    QString m_strUserId{""};

};
