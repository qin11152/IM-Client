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
    //����ģʽ��ȡ����Ψһ��ʽ
    static SingletonPtr Instance();
    ~DataBaseDelegate() override;
    DataBaseDelegate(DataBaseDelegate& l) = delete;
    DataBaseDelegate& operator=(DataBaseDelegate& l) = delete;
    //Ϊ���ݿ������û���id
    void setUserId(const QString& id);
    //����id��ȡ�����¼����
    int getChatRecordCountFromDB(const QString& id)const;
    //������ĳ���û��������¼��
    bool createUserChatTable(const QString& userid)const;
    //�����ϴ������
    bool createLastChatListTable()const;
    //����ͷ����Ϣ��
    bool createProfileImageTable()const;
    //������������ı�
    bool createFriendRequestTable()const;
    //��������ͷ��ʱ�����
    bool createFriendImageTimeStampTable()const;
    //����id��ȡ���һ�������¼
    QString queryLastChatRecord(const QString& id)const;
    //ֻ��id
    bool insertLastChat(const QString& id)const;
    //���뵽�ϴ��������
    bool insertLastChat(const std::vector<QString>& order)const;
    //�������ͷ��ʱ���
    bool insertFriendImageTimeStamp(const QString& id, const QString& friendId, const QString& time, const QString& imagePath)const;
    //����ϴ������
    bool clearLastChat()const;
    //���������¼
    bool insertChatRecoed(int TotalCount, const QString& userid, const QString& message, const QString& time,
                          bool isSelf, const QString& name)const;
    //������Ӻ�������
    bool insertAddFriendRequest(const QString& id, const QString& name, const QString& verifyMsg)const;
    //�������ͷ��·����ʱ���
    bool insertProfilePathAndTimestamp(const QString& id, const QString& path, const QString& timestamp)const;
    //�жϱ��Ƿ����
    bool isTableExist(const QString& tableName)const;
    //�����ݿ��в��ҳ��ϴιر�ʱ�������б�
    bool queryLastChatListFromDB(std::vector<MyLastChatFriendInfo>& m_tmpVec)const;
    //����id���������¼
    bool queryChatRecordAcodIdFromDB(QString id, std::vector<MyChatMessageInfo>& chatMessage, int queryCount,
                                     int beginPos)const;
    bool queryInitialAcordIdFromDB(const QString& id, QString& str)const;
    //��ȡ����������Ϣ����������Ӻ�δ���
    bool queryAddFriendInfoFromDB(QString id, std::vector<MyAddFriendInfo>& addFriendInfo)const;
    //����nameȥ���Һ��������Ӧ��id
    bool queryFriendRequestAcordName(const QString& name, QString& id)const;
    //��ѯ����ͷ��·��
    bool queryProfileImagePath(const QString& id, QString& path)const;
    //��ѯͷ��������id�Ƿ����
    bool queryIsIdExistInProfile(const QString& id)const;
    //��ȡ���к��ѵ�ͷ��ʱ���
    bool queryProfileTimeStamp(std::unordered_map<std::string, std::string>& mapTimeStamp)const;
    //����name����Ӧ�ĺ���������Ϊtrue
    bool updateFriendRequestStateAcordName(const QString& name)const;
    //���º���ͷ��ʱ���
    bool updateFriendImageTimestamp(QString& id,std::pair<QString, QString>& newInfo)const;
    //���º���ͷ��·��
    bool updateProfileImagePath(const QString& id, const QString& path)const;
    //����ͷ��·����ʱ���
    bool updateProfilleImagePathAndTimeStamp(const QString& id, const QString& path, const QString& timeStamp)const;
    //ɾ�����ڵĺ�������,�������30��Ϊ����,�����Ƿ�ͬ�ⶼɾ��
    bool deleteExpiredFriendRequest()const;
    //ɾ��lastchat�е�����
    bool deleteLastChatInfo()const;
    //��ʼ�����ӣ������⣬���
    void init();
    //�Ͽ�����
    void disConnect();
    
private:
    /**
     * bref.��ʼ��������ݿ⣬�鿴��û��һЩ����ı�û�оʹ���
     * 
     */
    void initTables();

private:
    DataBaseDelegate(QObject* parent = nullptr);
    static std::mutex m_mutex;
    static SingletonPtr m_SingletonPtr;
    QSqlDatabase m_dataBase;
    //���ݿ�ҲӦ��id��ָ������ͬ�û���¼������ͬ���ݿ�
    QString m_strUserId{""};

};
