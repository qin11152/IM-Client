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
    bool createUserChatTable(const QString& userid);
    bool createLastChatListTable();
    bool createFriendRequestTable();
    bool insertChatRecoed(int TotalCount,const QString& userid, const QString& message, const QString& time,bool isSelf,const QString& name);
    bool isTableExist(const QString& tableNmae);
    bool queryLastChatListFromDB(std::vector<QString>& m_tmpVec);
    bool queryChatRecordAcodIdFromDB(QString id, std::vector<MyChatMessageInfo>& chatMessage,int queryCount,int beginPos);
    bool QueryInitialAcordIdFromDB(QString id, QString& str);
    //获取好友请求信息，包括已添加和未添加
    bool queryAddFriendInfoFromDB(QString id,std::vector<MyAddFriendInfo>&addFriendInfo);
    bool queryFriendRequestAcordId(QString id);

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


