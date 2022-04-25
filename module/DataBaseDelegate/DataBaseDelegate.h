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

struct MyAddFriendInfo
{
public:
    QString m_strFriendName;
    QString m_strFriendId;
    QString m_strVerifyMsg;
    bool m_bIsValid{ false };
    MyAddFriendInfo(QString name, QString id, QString verifyMsg, bool isValid)
        :m_strFriendName(name),
        m_strFriendId(id),
        m_strVerifyMsg(verifyMsg),
        m_bIsValid(isValid)
    {}
};

class DataBaseDelegate : public QObject
{
    Q_OBJECT
    using SingletonPtr = std::shared_ptr<DataBaseDelegate>;

public:
    //����ģʽ��ȡ����Ψһ��ʽ
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
    //��ȡ����������Ϣ����������Ӻ�δ���
    bool QueryAddFriendInfoFromDB(int id,std::vector<MyAddFriendInfo>&addFriendInfo);

private:
    DataBaseDelegate(QObject* parent=nullptr);
    static std::mutex m_mutex;
    static SingletonPtr m_SingletonPtr;
    QSqlDatabase m_dataBase;
    //���ݿ�ҲӦ��id��ָ������ͬ�û���¼������ͬ���ݿ�
    int m_iId{ -1 };

    //��ʼ�����ӣ������⣬���
    void init();
};


