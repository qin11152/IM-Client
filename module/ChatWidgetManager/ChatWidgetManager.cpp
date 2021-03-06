#include "ChatWidgetManager.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/TCPConnect/TCPConnect.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "protocol/InitialRequestJsonData/InitialRequestJsonData.h"
#include "protocol/GetFriendListReplyData/GetFriendListReplyData.h"
#include "protocol/AddFriendResponseJsonData/AddFriendResponseJsonData.h"
#include "protocol/AddFriendRequestJsonData/AddFriendRequestJsonData.h"
#include "protocol/AddFriendNotifyJsonData/AddFriendNotifyJsonData.h"
#include "protocol/GetFriendListJsonData/GetFriendListJsonData.h"
#include <algorithm>

using SingletonPtr = std::shared_ptr<ChatWidgetManager>;
//初始化静态成员函数
SingletonPtr ChatWidgetManager::m_SingletonPtr = nullptr;

std::mutex ChatWidgetManager::m_mutex;

SingletonPtr ChatWidgetManager::Instance()
{
    //双重保护，第一重判空，避免生成多个
    if (m_SingletonPtr == nullptr)
    {
        std::lock_guard<std::mutex> lck(m_mutex);
        //加锁后判空，避免多线程生成多个实例
        if (m_SingletonPtr == nullptr)
        {
            m_SingletonPtr = std::shared_ptr<ChatWidgetManager>(new ChatWidgetManager);
        }
    }
    //返回指针
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

void ChatWidgetManager::initDBThreadConnect()
{
    if(!m_ptrDBOperateThread)
    {
        m_ptrDBOperateThread = new DatabaseOperateThread();
    }
    m_ptrDBOperateThread->init();
}

void ChatWidgetManager::setLastChatList(QStringList& m_lastChatList) const
{
    m_ptrDBOperateThread->setLastChatList(m_lastChatList);
}

void ChatWidgetManager::onSignalRecvFriendList(const QString& friendList, std::unordered_map<QString, int>& mapUserInfo,
                                               std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC)
{
    GetFriendListReplyData getFriendListReplyData(friendList.toStdString());
    for (auto& item : getFriendListReplyData.m_vecFriendList)
    {
        //添加到qml页面listmodel中
        QString strShouZiMu = convertToPinYin(QString::fromStdString(item.m_strFriendName)).toUpper().mid(0, 1);
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
    //从数据库中找到这个名字对应的id，然后返回给服务器
    DataBaseDelegate::Instance()->queryFriendRequestAcordName(friendName, strId);
    AddFriendResponseJsonData addFriendResponseJsonData;
    addFriendResponseJsonData.m_strMyId = m_strUserId.toStdString();
    addFriendResponseJsonData.m_strFriendId = strId.toStdString();
    addFriendResponseJsonData.m_bResult = true;
    //发送给服务器
    TCPConnect::Instance()->sendMessage(addFriendResponseJsonData.generateJson());
    //修改注册表中的状态为true
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
    //通知界面改变，上次聊天的界面添加一个新的好友项
    const AddFriendNotify addFriendNotifyData(msg.toStdString());
    MyFriendInfoWithFirstC tmp;
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
        tmp.m_strFirstChacter = convertToPinYin(QString::fromStdString(addFriendNotifyData.m_strName2)).mid(0, 1).
            toStdString();
    }
    else
    {
        tmp.m_strName = addFriendNotifyData.m_strName1;
        tmp.m_strFirstChacter = convertToPinYin(QString::fromStdString(addFriendNotifyData.m_strName1)).mid(0, 1).
            toStdString();
    }
    emit signalBecomeFriend(tmp);
}

void ChatWidgetManager::onSignalNewFriendRequest(const QString& msg)
{
    //写入数据库
    const AddFriendRequestJsonData addFriendRequestData(msg.toStdString().c_str());
    const char* name = addFriendRequestData.m_strName.c_str();
    const char* friendId = addFriendRequestData.m_strFriendId.c_str();
    const char* verifyMsg = addFriendRequestData.m_strVerifyMsg.c_str();
    //添加好友的界面插入一个新的
    DataBaseDelegate::Instance()->insertAddFriendRequest(friendId, name, verifyMsg);
    QMetaObject::invokeMethod(m_ptrAddFriendQMLRoot, "insertNewAddFriendRequest",
                              Q_ARG(QVariant, convertToPinYin(name).mid(0, 1)), Q_ARG(QVariant, name),
                              Q_ARG(QVariant, verifyMsg), Q_ARG(QVariant, false));
}

void ChatWidgetManager::onSignalUpdateLastChat()
{
    //由子线程去处理
    if (!m_ptrDBOperateThread)
    {
        m_ptrDBOperateThread = new DatabaseOperateThread();
        m_ptrDBOperateThread->init();
    }

    QStringList tmpOrder;
    onSignalGetModelOrder(tmpOrder);
    setLastChatList(tmpOrder);

    //设置操作类型为更新上次聊天数据库
    m_ptrDBOperateThread->setOperateType(DatabaseOperateType::UpdateLastChat);
    m_ptrDBOperateThread->start();
}

void ChatWidgetManager::onSignalGetModelOrder(QStringList& modelOrder)
{
    QVariant tmp;
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "getModelInfo", Q_RETURN_ARG(QVariant, tmp));
    modelOrder = tmp.toStringList();
}

ChatWidgetManager::ChatWidgetManager(QObject* parent)
    : QObject(parent)
{
    m_ptrDBOperateThread = new DatabaseOperateThread(nullptr);
}

ChatWidgetManager::~ChatWidgetManager()
{
    delete m_ptrDBOperateThread;
    m_ptrDBOperateThread = nullptr;
}

void ChatWidgetManager::setQMLRootPtr(QObject* AddFriendQMLRoot, QObject* FriendListQMLRoot, QObject* LastChatQMLRoot)
{
    m_ptrLastChatQMLRoot = LastChatQMLRoot;
    m_ptrFriendListQMLRoot = FriendListQMLRoot;
    m_ptrAddFriendQMLRoot = AddFriendQMLRoot;
    m_ptrDBOperateThread->setLastChatQML(m_ptrLastChatQMLRoot);
}

void ChatWidgetManager::initDBOperateThread()
{
    m_ptrDBOperateThread=new DatabaseOperateThread(nullptr);
    //为子线程设定id，保证操作的数据库正确
    m_ptrDBOperateThread->setCurUserId(m_strUserId);
}

void ChatWidgetManager::getFriendList()
{
    GetFriendListJsonData getFriendListData;
    getFriendListData.m_strUserId = m_strUserId.toStdString();
    const auto tmpStr = getFriendListData.generateJson();
    TCPConnect::Instance()->sendMessage(tmpStr);
}

//向服务器发送初始化消息，告知此id在线
void ChatWidgetManager::notifyServerOnline()
{
    InitialRequestJsonData initialJosnData;
    initialJosnData.m_strId = m_strUserId.toStdString();
    std::string sendMessage = initialJosnData.generateJson();
    TCPConnect::Instance()->sendMessage(sendMessage);
}

void ChatWidgetManager::getLastChatListFromDB(std::vector<MyLastChatFriendInfo>& vecLastChatFriend)
{
    //map结构体，存储上次关闭时聊天列表中的顺序
    DataBaseDelegate::Instance()->queryLastChatListFromDB(vecLastChatFriend);
}

std::vector<MyChatMessageInfo> ChatWidgetManager::getChatMessageAcordIdAtInit(QString strId)
{
    //先看这个id的聊天记录有多少
    int iMessageCount = DataBaseDelegate::Instance()->getChatRecordCountFromDB(strId);
    //如果大于10条就加载十条，小于十条就有多少加载多少
    int needLoadCount = (std::min)(10, iMessageCount);

    //获取聊天记录
    std::vector<MyChatMessageInfo> vecMyChatMessageInfo;

    if (0 == needLoadCount)
    {
        return vecMyChatMessageInfo;
    }
    //查询聊天记录的起始位置是聊天页面当前的数量,查询的数量就是needcount
    DataBaseDelegate::Instance()->queryChatRecordAcodIdFromDB(strId, vecMyChatMessageInfo, needLoadCount, 0);
    return vecMyChatMessageInfo;
}
