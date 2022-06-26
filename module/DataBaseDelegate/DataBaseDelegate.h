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
    //������ĳ���û��������¼��
    bool createUserChatTable(const QString& userid);
    //�����ϴ������
    bool createLastChatListTable();
    //������������ı�
    bool createFriendRequestTable();
    //����lastchat���ݱ�
    bool createLastChatBackUp();
    //��lastchat�в����¼
    bool insertLastChat(QString id, QString pos);
    //ֻ��id
    bool insertLastChat(QString id);
    //���������¼
    bool insertChatRecoed(int TotalCount,const QString& userid, const QString& message, const QString& time,bool isSelf,const QString& name);
    //������Ӻ�������
    bool insertAddFriendRequest(const QString& id,const QString& name,const QString& verifyMsg);
    //�жϱ��Ƿ����
    bool isTableExist(const QString& tableNmae);
    //�����ݿ��в��ҳ��ϴιر�ʱ�������б�
    bool queryLastChatListFromDB(std::vector<MyLastChatFriendInfo>& m_tmpVec);
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
    //ɾ��lastchat�е�����
    bool deleteLastChatInfo();
    //copylastchat�������У�Ȼ�����lastchat
    bool copyLastChatToBackUps();

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


