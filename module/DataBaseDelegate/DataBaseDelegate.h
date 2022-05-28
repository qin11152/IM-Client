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
    void SetUserId(QString id);
    int GetChatRecordCountFromDB(QString id);
    bool createUserChatTable(const QString& userid);
    bool createLastChatListTable();
    bool createFriendRequestTable();
    //���������¼
    bool insertChatRecoed(int TotalCount,const QString& userid, const QString& message, const QString& time,bool isSelf,const QString& name);
    //�жϱ��Ƿ����
    bool isTableExist(const QString& tableNmae);
    //�����ݿ��в��ҳ��ϴιر�ʱ�������б�
    bool queryLastChatListFromDB(std::vector<QString>& m_tmpVec);
    //����id���������¼
    bool queryChatRecordAcodIdFromDB(QString id, std::vector<MyChatMessageInfo>& chatMessage,int queryCount,int beginPos);
    bool QueryInitialAcordIdFromDB(QString id, QString& str);
    //��ȡ����������Ϣ����������Ӻ�δ���
    bool queryAddFriendInfoFromDB(QString id,std::vector<MyAddFriendInfo>&addFriendInfo);
    //����nameȥ���Һ��������Ӧ��id
    bool queryFriendRequestAcordName(QString name,QString& id);
    //����name����Ӧ�ĺ���������Ϊtrue
    bool updateFriendRequestStateAcordName(QString name);
    //ɾ�����ڵĺ�������,�������30��Ϊ����,�����Ƿ�ͬ�ⶼɾ��
    bool deleteExpiredFriendRequest();

private:
    DataBaseDelegate(QObject* parent=nullptr);
    static std::mutex m_mutex;
    static SingletonPtr m_SingletonPtr;
    QSqlDatabase m_dataBase;
    //���ݿ�ҲӦ��id��ָ������ͬ�û���¼������ͬ���ݿ�
    QString m_strUserId{ "" };

    //��ʼ�����ӣ������⣬���
    void init();
};


