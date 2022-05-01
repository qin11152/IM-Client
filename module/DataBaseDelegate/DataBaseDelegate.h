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
    //����ģʽ��ȡ����Ψһ��ʽ
    static SingletonPtr Instance();
    ~DataBaseDelegate();
    DataBaseDelegate(DataBaseDelegate& l) = delete;
    DataBaseDelegate& operator=(DataBaseDelegate& l) = delete;
    void SetUserId(int id);
    int GetChatRecordCountFromDB(int id);
    bool createUserChatTable(const QString& userid);
    bool createLastChatListTable();
    bool createFriendRequestTable();
    bool insertChatRecoed(int TotalCount,const QString& userid, const QString& message, const QString& time,bool isSelf,const QString& name);
    bool isTableExist(const QString& tableNmae);
    bool queryLastChatListFromDB(std::map<int,int>& m_tmpMap);
    bool queryChatRecordAcodIdFromDB(int id, std::vector<MyChatMessageInfo>& chatMessage,int queryCount,int beginPos);
    bool QueryInitialAcordIdFromDB(int id, QString& str);
    //��ȡ����������Ϣ����������Ӻ�δ���
    bool queryAddFriendInfoFromDB(int id,std::vector<MyAddFriendInfo>&addFriendInfo);
    bool queryFriendRequestAcordId(QString id);

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


