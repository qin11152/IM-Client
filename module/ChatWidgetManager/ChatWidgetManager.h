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
    void getLastChatListFromDB(std::vector<QString>& vecLastChatFriend);

    //��ʼ����ʱ���ȡ�����¼��10������С��10��
    std::vector<MyChatMessageInfo> getChatMessageAcordIdAtInit(QString strId);
    //�յ������б���Ϣ��
    void onSignalRecvFriendList(const QString& friendList, std::map<QString, int>& mapUserInfo, std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC);
    //�յ�qmlҳ��ͬ����Ӻ��ѵ�����
    void onSignalAgreeAddFriend(QString friendName);


signals:
    void signalGetFriendListFinished();

private:
    //��ʼ������
    void initConnect();

private:
    ChatWidgetManager(QObject* parent = nullptr);
    static std::mutex m_mutex;                  //������֤�̰߳�ȫ
    static SingletonPtr m_SingletonPtr;         //���������ָ��
    QString m_strUserId{ "" };                  //�Լ���id
};
