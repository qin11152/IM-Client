#pragma once

#include "../module/MyDefine.h"
#include <QObject>
#include <QSqlDatabase>
#include <memory>
#include <mutex>
#include <map>
#include <vector>

struct MyChatMessageInfo
{
public:
    QString m_strMessage;
    QString m_strName;
    QString m_strTime;
    bool m_bIsSelf;
    MyChatMessageInfo(QString message, QString name, QString time, bool isSelf) :
        m_strMessage(message),
        m_strName(name),
        m_strTime(time),
        m_bIsSelf(isSelf)
    {}
};

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
    void SetUserId(int id);
    int GetChatRecordCountFromDB(int id);
    bool createUserChatTable(const QString& userid);
    bool createLastChatListTable();
    bool insertChatRecoed(const QString& userid, const QString& message, const QString& time);
    bool isTableExist(const QString& tableNmae);
    bool queryLastChatListFromDB(std::map<int,int>& m_tmpMap);
    bool queryChatRecordAcodIdFromDB(int id, std::vector<MyChatMessageInfo>& chatMessage,int queryCount);
    bool QueryInitialAcordIdFromDB(int id, QString& str);

private:
    DataBaseDelegate(QObject* parent=nullptr);
    static std::mutex m_mutex;
    static SingletonPtr m_SingletonPtr;
    QSqlDatabase m_dataBase;
    //数据库也应由id来指定，不同用户登录建立不同数据库
    int m_iId{ -1 };

    //初始化连接，创建库，表等
    void init();
};


