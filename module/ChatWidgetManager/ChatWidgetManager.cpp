#include "ChatWidgetManager.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/TCPConnect/TCPConnect.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "protocol/InitialRequestJsonData/InitialRequestJsonData.h"
#include "protocol/GetFriendListReplyData/GetFriendListReplyData.h"
#include "protocol/AddFriendResponseJsonData/AddFriendResponseJsonData.h"
#include "protocol/AddFriendRequestJsonData/AddFriendRequestJsonData.h"
#include "protocol/AddFriendNotifyJsonData/AddFriendNotifyJsonData.h"
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

void ChatWidgetManager::setUserName(QString& name)
{
    m_strUserName = name;
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

void ChatWidgetManager::onSignalAgreeAddFriend(QString friendName)
{
    QString strId = "";
    //TODO�����ݿ����ҵ�������ֶ�Ӧ��id��Ȼ�󷵻ظ�������
    DataBaseDelegate::Instance()->queryFriendRequestAcordName(friendName, strId);
    AddFriendResponseJsonData addFriendResponseJsonData;
    addFriendResponseJsonData.m_strMyId = m_strUserId.toStdString();
    addFriendResponseJsonData.m_strFriendId = strId.toStdString();
    addFriendResponseJsonData.m_bResult = true;
    //���͸�������
    TCPConnect::Instance()->sendMessage(addFriendResponseJsonData.generateJson());
    //�޸�ע����е�״̬Ϊtrue
    DataBaseDelegate::Instance()->updateFriendRequestStateAcordName(friendName);
}

void ChatWidgetManager::onSignalRequestAddFriend(QString friendId, QString verifyMsg)
{
    AddFriendRequestJsonData addFriendRequestData;
    addFriendRequestData.m_strFriendId = friendId.toStdString();
    addFriendRequestData.m_strVerifyMsg = verifyMsg.toStdString();
    addFriendRequestData.m_strMyId = m_strUserId.toStdString();

    TCPConnect::Instance()->sendMessage(addFriendRequestData.generateJson());
}

void ChatWidgetManager::onSignalBecomeFriend(const QString& msg)
{
    //֪ͨ����ı䣬�ϴ�����Ľ������һ���µĺ�����
    AddFriendNotify addFriendNotifyData(msg.toStdString());
    MyFriendInfoWithFirstC tmp;
    qDebug() << "qqqq my id is:" << m_strUserId;
    qDebug() << "qqqq my name is:" << m_strUserName;
    if (m_strUserId.toStdString() == addFriendNotifyData.m_strId1)
    {
        tmp.m_strId = addFriendNotifyData.m_strId2;
    }
    else
    {
        tmp.m_strId = addFriendNotifyData.m_strId1;
    }
    if (m_strUserName.toStdString() == addFriendNotifyData.m_strName1)
    {
        tmp.m_strName = addFriendNotifyData.m_strName2;
        tmp.m_strFirstChacter = convertToPinYin(QString::fromStdString(addFriendNotifyData.m_strName2)).mid(0,1).toStdString();
    }
    else
    {
        tmp.m_strName = addFriendNotifyData.m_strName1;
        tmp.m_strFirstChacter = convertToPinYin(QString::fromStdString(addFriendNotifyData.m_strName1)).mid(0, 1).toStdString();
    }
    emit signalAddFriendToLastChat(tmp);
}

void ChatWidgetManager::onSignalNewFriendRequest(const QString& msg)
{
    //д�����ݿ�
    AddFriendRequestJsonData addFriendRequestData(msg.toStdString().c_str());
    const char* name = addFriendRequestData.m_strName.c_str();
    const char* friendId = addFriendRequestData.m_strFriendId.c_str();
    const char* verifyMsg = addFriendRequestData.m_strVerifyMsg.c_str();
    //��Ӻ��ѵĽ������һ���µ�
    DataBaseDelegate::Instance()->insertAddFriendRequest(friendId, name, verifyMsg);
    QMetaObject::invokeMethod(m_ptrAddFriendQMLRoot, "insertNewAddFriendRequest", Q_ARG(QVariant, convertToPinYin(name).mid(0, 1)), Q_ARG(QVariant, name), Q_ARG(QVariant, verifyMsg), Q_ARG(QVariant, false));
}

ChatWidgetManager::ChatWidgetManager(QObject *parent)
    : QObject(parent)
{
}

ChatWidgetManager::~ChatWidgetManager()
{
}

void ChatWidgetManager::setQMLRootPtr(QObject* AddFriendQMLRoot, QObject* FriendListQMLRoot, QObject* LastChatQMLRoot)
{
    m_ptrLastChatQMLRoot = LastChatQMLRoot;
    m_ptrFriendListQMLRoot = FriendListQMLRoot;
    m_ptrAddFriendQMLRoot = AddFriendQMLRoot;
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

    if (0 == needLoadCount)
    {
        return vecMyChatMessageInfo;
    }
    //��ѯ�����¼����ʼλ��������ҳ�浱ǰ������,��ѯ����������needcount
    DataBaseDelegate::Instance()->queryChatRecordAcodIdFromDB(strId, vecMyChatMessageInfo, needLoadCount, 0);
    return vecMyChatMessageInfo;
}
