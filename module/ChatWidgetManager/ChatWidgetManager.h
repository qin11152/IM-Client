/************************************************************************/
/* ��������߼����Ź����࣬����߼������������ڴ�                                                                   
/************************************************************************/
#pragma once

#include "../MyDefine.h"
#include "module/LastChatInfoUpdateThread/DatabaseOperateThread.h"
#include "module/ThreadPool/ThreadPool.h"
#include <QObject>
#include <mutex>
#include <memory>
#include <vector>

class ChatWidgetManager : public QObject
{
    Q_OBJECT

public:
    using SingletonPtr = std::shared_ptr<ChatWidgetManager>;
    //����ģʽ��Ψһ��ȡ��ָ��ķ�����ʹ��������ָ�룬�Զ��ڴ����
    static SingletonPtr Instance();
    //���追������͸�ֵ���캯��
    ChatWidgetManager(const ChatWidgetManager&) = delete;
    ChatWidgetManager& operator=(const ChatWidgetManager&) = delete;
    ~ChatWidgetManager()override;

    //����id
    void setUserId(QString strId);
    //��������
    void setUserName(QString& name);
    //����qml������ָ�룬�Ա����qml�еĺ���
    void setQMLRootPtr(QObject* AddFriendQMLRoot, QObject* FriendListQMLRoot, QObject* LastChatQMLRoot);
    //��ʼ�����ݿ�������߳�
    void initDBOperateThread();
    //��ʼ�����ݿ�������߳�����
    void initDBThreadConnect();
    //�������߳������µ�lastchat�б�
    void setLastChatList(QStringList& m_lastChatList)const;

    //�ӷ���˻�ȡ��ǰ�û��ĺ����б�
    void getFriendList();
    //֪ͨ����˿ͻ�������
    void notifyServerOnline();
    //��ȡ�ϴ�����ĺ����б�
    void getLastChatListFromDB(std::vector <MyLastChatFriendInfo>& vecLastChatFriend);
    //����ȡ���ĺ���ʱ��������ݿ��еĽ��жԱ�
    void compareImageTimestap(std::vector<MyFriendInfoWithFirstC> vecFriendInfo);

    //��ʼ����ʱ���ȡ�����¼��10������С��10��
    std::vector<MyChatMessageInfo> getChatMessageAcordIdAtInit(QString strId);
    //�յ������б���Ϣ��

signals:
    //������Ϣ�ź�
    void signalSendMsg(std::string msg);
    //����ͼƬ��Ϣ�ź�
    void signalSendImageMsg(QString& strBase64Image, const QString& imageName);

public slots:

    //����˷����������б�
    void onSignalRecvFriendList(const QString& friendList, std::unordered_map<QString, int>& mapUserInfo, std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC);
    //�յ�qmlҳ��ͬ����Ӻ��ѵ�����
    void onSignalAgreeAddFriend(QString friendName);
    //qmlҳ�淢���������
    void onSignalRequestAddFriend(QString friendId, QString verifyMsg);
    //������֪ͨ��Ϊ�˺���
    void onSignalBecomeFriend(const QString& msg);
    //������֪ͨ���˼Ӻ���
    void onSignalNewFriendRequest(const QString& msg);
    //Ҫ���޸����ݿ���lastchat������
    void onSignalUpdateLastChat();
    //Ҫ���lastchat��qmlҳ���л�ȡ��ǰ��˳��
    void onSignalGetModelOrder(QStringList& modelOrder);

signals:
    void signalGetFriendListFinished();
    //�к���ͬ��������,���͸�chatwidget���棬�������½���
    void signalBecomeFriend(const MyFriendInfoWithFirstC& friendInfo);

private:
    ChatWidgetManager(QObject* parent = nullptr);
    static std::mutex m_mutex;                  //������֤�̰߳�ȫ
    static SingletonPtr m_SingletonPtr;         //���������ָ��
    QString m_strUserId{ "" };                  //�Լ���id
    QString m_strUserName{ "" };                //�Լ����ǳ�
    ThreadPool* m_ptrThreadPool{ nullptr };     //�̳߳�

    QObject* m_ptrLastChatQMLRoot{ nullptr }; //�ϴ�����qml�ĸ�����
    QObject* m_ptrFriendListQMLRoot{ nullptr };//�����б�qml�ĸ�����
    QObject* m_ptrAddFriendQMLRoot{ nullptr };  //��Ӻ���qml����ĸ�����

    DatabaseOperateThread* m_ptrDBOperateThread{ nullptr };  //���ڲ���lastchat���ݿ�����߳�

    void initConnect();         //��ʼ���źŲ�����
};
