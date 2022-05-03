#include "ChatWidgetManager.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/TCPConnect/TCPConnect.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "protocol/InitialRequestJsonData/InitialRequestJsonData.h"
#include "protocol/GetFriendListReplyData/GetFriendListReplyData.h"
#include <algorithm>

using SingletonPtr = std::shared_ptr<ChatWidgetManager>;
//��ʼ����̬��Ա����
SingletonPtr ChatWidgetManager::m_SingletonPtr = nullptr;

std::mutex ChatWidgetManager::m_mutex;

SingletonPtr ChatWidgetManager::Instance()
{
    //˫�ر�������һ���пգ��������ɶ��
    if (m_SingletonPtr == nullptr)
    {
        std::lock_guard<std::mutex> lck(m_mutex);
        //�������пգ�������߳����ɶ��ʵ��
        if (m_SingletonPtr == nullptr)
        {
            m_SingletonPtr = std::shared_ptr<ChatWidgetManager>(new ChatWidgetManager);
        }
    }
    //����ָ��
    return m_SingletonPtr;
}

void ChatWidgetManager::setUserId(QString strId)
{
    m_strUserId = strId;
}

void ChatWidgetManager::initConnect()
{
    //connect(TCPConnect::Instance().get(), &TCPConnect::signalRecvFriendListMessage, this, &ChatWidgetManager::onSignalRecvFriendList);
}

void ChatWidgetManager::onSignalRecvFriendList(const QString& friendList, std::map<QString, int>& mapUserInfo, std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC)
{
    GetFriendListReplyData getFriendListReplyData(friendList.toStdString());
    for (auto& item : getFriendListReplyData.m_vecFriendList)
    {
        //��ӵ�qmlҳ��listmodel��
        QString strShouZiMu = convertToPinYin(QString::fromStdString(item.m_strFriendName)).toUpper().mid(0,1);
        MyFriendInfoWithFirstC tmpFriendInfo;
        tmpFriendInfo.m_strFirstChacter = strShouZiMu.toStdString();
        tmpFriendInfo.m_strId = item.m_strFriendId;
        tmpFriendInfo.m_strImagePath = "";
        tmpFriendInfo.m_strName = item.m_strFriendName;
        vecFriendInfoWithC.push_back(tmpFriendInfo);
        mapUserInfo[QString::fromStdString(item.m_strFriendId)] = vecFriendInfoWithC.size() - 1;
    }
    emit signalGetFriendListFinished();
}

ChatWidgetManager::ChatWidgetManager(QObject *parent)
    : QObject(parent)
{
}

ChatWidgetManager::~ChatWidgetManager()
{
}

void ChatWidgetManager::getFriendList()
{
    GetFriendListJsonData getFriendListData;
    getFriendListData.m_strUserId = m_strUserId.toStdString();
    auto tmpStr = getFriendListData.generateJson();
    TCPConnect::Instance()->sendMessage(tmpStr);
    initConnect();
}

//����������ͳ�ʼ����Ϣ����֪��id����
void ChatWidgetManager::notifyServerOnline()
{
    InitialRequestJsonData initialJosnData;
    initialJosnData.m_strId = m_strUserId.toStdString();
    std::string sendMessage = initialJosnData.generateJson();
    TCPConnect::Instance()->sendMessage(sendMessage);
}

void ChatWidgetManager::getLastChatListFromDB(std::vector<QString>& vecLastChatFriend)
{
    //map�ṹ�壬�洢�ϴιر�ʱ�����б��е�˳��
    std::map<QString, QString> LastChatInfo;
    DataBaseDelegate::Instance()->queryLastChatListFromDB(vecLastChatFriend);
}

std::vector<MyChatMessageInfo> ChatWidgetManager::getChatMessageAcordIdAtInit(QString strId)
{
    //�ȿ����id�������¼�ж���
    int iMessageCount = DataBaseDelegate::Instance()->GetChatRecordCountFromDB(strId);
    //�������10���ͼ���ʮ����С��ʮ�����ж��ټ��ض���
    int needLoadCount = (std::min)(10, iMessageCount);
    //��ȡ�����¼
    std::vector<MyChatMessageInfo> vecMyChatMessageInfo;
    //��ѯ�����¼����ʼλ��������ҳ�浱ǰ������,��ѯ����������needcount
    DataBaseDelegate::Instance()->queryChatRecordAcodIdFromDB(strId, vecMyChatMessageInfo, needLoadCount, 0);
    return vecMyChatMessageInfo;
}
