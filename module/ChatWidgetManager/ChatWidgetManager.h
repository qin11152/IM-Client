/************************************************************************/
/* ��������߼����Ź����࣬����߼������������ڴ�                                                                   
/************************************************************************/
#pragma once

#include "../MyDefine.h"
#include "protocol/GetFriendListJsonData/GetFriendListJsonData.h"
#include <QObject>
#include <mutex>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>

class ChatWidgetManager : public QObject
{
    Q_OBJECT

public:
    using SingletonPtr = std::shared_ptr<ChatWidgetManager>;
    //����ģʽ��Ψһ��ȡ��ָ��ķ�����ʹ��������ָ�룬�Զ��ڴ����
    static SingletonPtr Instance();
    //����id
    void setUserId(QString strId);
    //���追������͸�ֵ���캯��
    ChatWidgetManager(const ChatWidgetManager&) = delete;
    ChatWidgetManager& operator=(const ChatWidgetManager&) = delete;
    ~ChatWidgetManager();

    //�ӷ���˻�ȡ��ǰ�û��ĺ����б�
    void getFriendList();
    //֪ͨ����˿ͻ�������
    void notifyServerOnline();
    //��ȡ�ϴ�����ĺ����б�
    void getLastChatListFromDB();

    //��ȡ�����б�
    std::vector<MyFriendInfoWithFirstC> getMyFriendVec()const { return m_vecFriendInfoWithC; }
    //��ʼ����ʱ���ȡ�����¼��10������С��10��
    std::vector<MyChatMessageInfo> getChatMessageAcordIdAtInit(QString strId);
    //��ȡ�ϴ������б�
    std::vector<QString> getLastChatList()const { return m_vecLastChatFriend; }
    //����id��ȡ������ѵ���Ϣ
    MyFriendInfoWithFirstC getFriendInfo(QString strId);

signals:
    void signalGetFriendListFinished();

private:
    //��ʼ������
    void initConnect();
    //�յ������б���Ϣ��
    void onSignalRecvFriendList(const QString& friendList);

private:
    ChatWidgetManager(QObject* parent = nullptr);
    static std::mutex m_mutex;                  //������֤�̰߳�ȫ
    static SingletonPtr m_SingletonPtr;         //���������ָ��
    QString m_strUserId{ "" };                  //�Լ���id

    //�洢������Ϣ��������ĸ
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;
    std::map<QString,int> m_mapUserInfo;     //�洢����id�͸�id��vec�ж�Ӧ��λ�ã��Ա������Ϣ
    std::vector<QString> m_vecLastChatFriend;        //�ϴ�����ҳ����ĺ���
};
