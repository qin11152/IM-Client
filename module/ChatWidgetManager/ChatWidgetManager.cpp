﻿#include "ChatWidgetManager.h"
#include "module/Log/Log.h"
#include "module/TCPThread/TCPThread.h"
#include "module/FileManager/FileManager.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/DataBaseDelegate/DatabaseOperateNeededFile.h"
#include "module/PublicDataManager/PublicDataManager.h"

#include "protocol/GetFriendListJsonData/GetFriendListJsonData.h"
#include "protocol/ChatMessageJsonData/SingleChatMessageJsonData.h"
#include "protocol/InitialRequestJsonData/InitialRequestJsonData.h"
#include "protocol/GetFriendListReplyData/GetFriendListReplyData.h"
#include "protocol/AddFriendNotifyJsonData/AddFriendNotifyJsonData.h"
#include "protocol/getProfileImageJsonData/getProfileImageJsonData.h"
#include "protocol/AddFriendRequestJsonData/AddFriendRequestJsonData.h"
#include "protocol/AddFriendResponseJsonData/AddFriendResponseJsonData.h"
#include "protocol/StartGroupChatReplyJsonData/StartGroupChatReplyJsonData.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QApplication>

namespace module
{
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
		if (!m_ptrDBOperateThread)
		{
			m_ptrDBOperateThread = new DatabaseOperateThread();
		}
		m_ptrDBOperateThread->init();
	}

	void ChatWidgetManager::setLastChatList(std::vector<std::pair<QString, bool>>& modelOrder) const
	{
		m_ptrDBOperateThread->setLastChatList(modelOrder);
	}

	void ChatWidgetManager::onSignalRecvFriendList(const QString& friendList, std::unordered_map<QString, int>& mapUserInfo,
		std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC)
	{
#if defined(QT_DEBUG)
		{
			qDebug() << "ChatWidgetManager::onSignalRecvFriendList";
			_LOG(Logcxx::Level::INFO, "ChatWidgetManager::onSignalRecvFriendList");
		}
#endif
	protocol::GetFriendListReplyData getFriendListReplyData(friendList.toStdString());
	for (auto& item : getFriendListReplyData.m_vecFriendList)
	{
#if defined(QT_DEBUG)
		qDebug() << "friend id:" << item.m_strFriendId.c_str() << ",name:" << item.m_strFriendName.c_str();
		_LOG(Logcxx::Level::INFO, "friend id:%s;name:%s。", item.m_strFriendId.c_str(), item.m_strFriendName.c_str());
#endif
		//添加到qml页面listmodel中
		QString strShouZiMu = Base::PinYin::convertToPinYin(QString::fromStdString(item.m_strFriendName)).toUpper().mid(0, 1);
		MyFriendInfoWithFirstC tmpFriendInfo;
		tmpFriendInfo.m_strFirstChacter = strShouZiMu.toStdString();
		tmpFriendInfo.m_strId = item.m_strFriendId;
		tmpFriendInfo.m_strName = item.m_strFriendName;
		tmpFriendInfo.m_strImageTimestamp = item.m_strFriendImageTimeStamp;
		tmpFriendInfo.m_bIsGroupChat = false;
		//从数据库获取头像路径
		QString strImage = "";
		database::ProfilePictureDatabase profilePictureDatabase;
		profilePictureDatabase.queryProfilePicturePath(item.m_strFriendId.c_str(), strImage);
		//如果数据库中没有头像路径，则使用默认头像
		if (strImage.isEmpty())
		{
			strImage = kDefaultProfileImage;
		}
		tmpFriendInfo.m_strImagePath = strImage.toStdString();
		vecFriendInfoWithC.push_back(tmpFriendInfo);
		mapUserInfo[QString::fromStdString(item.m_strFriendId)] = vecFriendInfoWithC.size() - 1;
	}
	compareImageTimestap(vecFriendInfoWithC);
	emit signalGetFriendListFinished();
	}

	void ChatWidgetManager::onSignalAgreeAddFriend(const QString& friendId)
	{
		protocol::AddFriendResponseJsonData addFriendResponseJsonData;
		addFriendResponseJsonData.m_strMyId = m_strUserId.toStdString();
		addFriendResponseJsonData.m_strFriendId = friendId.toStdString();
		addFriendResponseJsonData.m_bResult = true;
		//发送给服务器
		TCPThread::get_mutable_instance().sendMessage(addFriendResponseJsonData.generateJson());
		//修改注册表中的状态为true
		database::AddFriendDatabase addFriendDatabase;
		addFriendDatabase.updateFriendRequestStateAcordId(friendId);
	}

	void ChatWidgetManager::onSignalRequestAddFriend(QString friendId, QString verifyMsg)
	{
		protocol::AddFriendRequestJsonData addFriendRequestData;
		addFriendRequestData.m_strFriendId = friendId.toStdString();
		addFriendRequestData.m_strVerifyMsg = verifyMsg.toStdString();
		addFriendRequestData.m_strMyId = m_strUserId.toStdString();

		TCPThread::get_mutable_instance().sendMessage(addFriendRequestData.generateJson());
		//emit signalSendMsg(addFriendRequestData.generateJson());
	}

	void ChatWidgetManager::onSignalBecomeFriend(const QString& msg)
	{
		//通知界面改变，上次聊天的界面添加一个新的好友项
		const protocol::AddFriendNotify addFriendNotifyData(msg.toStdString());
		MyFriendInfoWithFirstC tmp;
		if (m_strUserId.toStdString() == addFriendNotifyData.m_strId1)
		{
			tmp.m_strId = addFriendNotifyData.m_strId2;
			tmp.m_strName = addFriendNotifyData.m_strName2;
			tmp.m_strFirstChacter = Base::PinYin::convertToPinYin(QString::fromStdString(addFriendNotifyData.m_strName2)).mid(0, 1).
				toStdString();
			tmp.m_strImagePath = addFriendNotifyData.m_strImageStamp1;
		}
		else
		{
			tmp.m_strId = addFriendNotifyData.m_strId1;
			tmp.m_strName = addFriendNotifyData.m_strName1;
			tmp.m_strFirstChacter = Base::PinYin::convertToPinYin(QString::fromStdString(addFriendNotifyData.m_strName1)).mid(0, 1).
				toStdString();
			tmp.m_strImagePath = addFriendNotifyData.m_strImageStamp2;
		}
		emit signalBecomeFriend(tmp);
	}

	void ChatWidgetManager::onSignalNewFriendRequest(const QString& msg)
	{
		//写入数据库
		const protocol::AddFriendRequestJsonData addFriendRequestData(msg.toStdString().c_str());
		const char* name = addFriendRequestData.m_strName.c_str();
		const char* friendId = addFriendRequestData.m_strFriendId.c_str();
		const char* verifyMsg = addFriendRequestData.m_strVerifyMsg.c_str();
		//添加好友的界面插入一个新的
		database::AddFriendDatabase addFriendDatabase;
		addFriendDatabase.insertAddFriendRequest(friendId, name, verifyMsg);
		QMetaObject::invokeMethod(m_ptrAddFriendQMLRoot, "insertNewAddFriendRequest",
			Q_ARG(QVariant, Base::PinYin::convertToPinYin(name).mid(0, 1)), Q_ARG(QVariant, name),
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

		std::vector<std::pair<QString, bool>> modelOrder;
		onSignalGetModelOrder(modelOrder);
		setLastChatList(modelOrder);

		//设置操作类型为更新上次聊天数据库
		m_ptrDBOperateThread->setOperateType(DatabaseOperateType::UpdateLastChat);
		m_ptrDBOperateThread->start();
	}

	void ChatWidgetManager::onSignalGetModelOrder(std::vector<std::pair<QString, bool>>& modelOrder)
	{
		QVariant id;
		QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "getModelInfo", Q_RETURN_ARG(QVariant, id));
		auto idList = id.toList();
		QVariant isGroup;
		QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "getModelGroupState", Q_RETURN_ARG(QVariant, isGroup));
		auto isGroupList = isGroup.toList();
		for (int i = 0; i < isGroupList.size(); ++i)
		{
			modelOrder.push_back(std::pair<QString, bool>(idList.at(i).toString(), isGroupList.at(i).toBool()));
		}
	}

	void ChatWidgetManager::onSignalStartGroupChatReply(const QString& msg)
	{
		//需要看下头像有没有，没有得获取好友头像
		protocol::StartGroupChatReplyJsonData startGroupChatReplyData(msg.toStdString());
		auto GroupVec = startGroupChatReplyData.m_vecGroupChatId;
		compareImageTimeStamp(GroupVec);

		//通知界面lastchat增加这个群聊
		MyFriendInfoWithFirstC friendInfo;
		friendInfo.m_bIsGroupChat = true;
		friendInfo.m_strId = startGroupChatReplyData.m_strGroupId;
		friendInfo.m_strName = startGroupChatReplyData.m_strGroupName;
		emit signalAddGroupChat2LastChat(friendInfo);
	}

	ChatWidgetManager::ChatWidgetManager(QObject* parent)
		: QObject(parent)
	{
		m_ptrDBOperateThread = new DatabaseOperateThread(nullptr);
		m_ptrThreadPool = new ThreadPool(10);
		m_ptrThreadPool->startPool();
		initConnect();
	}

	ChatWidgetManager::~ChatWidgetManager()
	{
		delete m_ptrDBOperateThread;
		m_ptrDBOperateThread = nullptr;
		if (m_ptrThreadPool)
		{
			m_ptrThreadPool->stopPool();
		}
		delete m_ptrThreadPool;
		m_ptrThreadPool = nullptr;
	}

	void ChatWidgetManager::setQMLRootPtr(QObject* AddFriendQMLRoot, QObject* FriendListQMLRoot, QObject* LastChatQMLRoot)
	{
		m_ptrLastChatQMLRoot = LastChatQMLRoot;
		m_ptrFriendListQMLRoot = FriendListQMLRoot;
		m_ptrAddFriendQMLRoot = AddFriendQMLRoot;
		m_ptrDBOperateThread->setLastChatQML(m_ptrLastChatQMLRoot);
		std::vector<std::pair<QString, bool>> modelOrder;
		onSignalGetModelOrder(modelOrder);
	}

	void ChatWidgetManager::initDBOperateThread()
	{
		m_ptrDBOperateThread = new DatabaseOperateThread(nullptr);
		//为子线程设定id，保证操作的数据库正确
		m_ptrDBOperateThread->setCurUserId(m_strUserId);
	}

	//查看一些必需的文件夹和文件是否存在
	void ChatWidgetManager::initDirAndFile()
	{
		//获取当前程序所在路径
		QString strAppPath = QCoreApplication::applicationDirPath();

		//查看data文件夹
		QString strDataPath = strAppPath + "/data";
		if (!Base::fileoperate::FileManager::get_mutable_instance().checkDirExist(strDataPath))
		{
			Base::fileoperate::FileManager::get_mutable_instance().createDir(strDataPath);
		}

		//查看对应id的文件夹是否存在
		QString idPath = strDataPath + "/" + m_strUserId;
		if (!Base::fileoperate::FileManager::get_mutable_instance().checkDirExist(idPath))
		{
			Base::fileoperate::FileManager::get_mutable_instance().createDir(idPath);
		}
		//把自己的id对应的文件夹路径保存下来
		PublicDataManager::get_mutable_instance().setIdDirPath(idPath);

		//查看image文件夹
		QString strImagePath = idPath + "/image";
		if (!Base::fileoperate::FileManager::get_mutable_instance().checkDirExist(strImagePath))
		{
			Base::fileoperate::FileManager::get_mutable_instance().createDir(strImagePath);
		}
	}

	void ChatWidgetManager::getFriendList()
	{
		protocol::GetFriendListJsonData getFriendListData;
		getFriendListData.m_strUserId = m_strUserId.toStdString();
		auto tmpStr = getFriendListData.generateJson();
		TCPThread::get_mutable_instance().sendMessage(tmpStr);
		//emit signalSendMsg(tmpStr);
	}

	//向服务器发送初始化消息，告知此id在线
	void ChatWidgetManager::notifyServerOnline()
	{
		protocol::InitialRequestJsonData initialJosnData;
		initialJosnData.m_strId = m_strUserId.toStdString();
		std::string sendMessage = initialJosnData.generateJson();
		//emit signalSendMsg(sendMessage);
		TCPThread::get_mutable_instance().sendMessage(sendMessage);
	}

	void ChatWidgetManager::getLastChatListFromDB(std::vector<MyLastChatFriendInfo>& vecLastChatFriend)
	{
		database::LastChatDatabase lastChatDatabase;
		//map结构体，存储上次关闭时聊天列表中的顺序
		lastChatDatabase.queryLastChatListFromDB(vecLastChatFriend);
	}

	void ChatWidgetManager::compareImageTimestap(std::vector<MyFriendInfoWithFirstC> vecFriendInfo)
	{
		auto dataBase = QSqlDatabase::addDatabase("QSQLITE", "sqlitecompareimage");
		QString dataName = QApplication::applicationDirPath() + "/data/chatinfo" + m_strUserId + ".db";
		dataBase.setDatabaseName(dataName);
		if (!dataBase.open())
		{
			qDebug() << "open database failed";
		}
		const QString str = "select id, timestamp from profileImage";
		std::unordered_map<std::string, std::string> mapTimeStamp;
		QSqlQuery query(dataBase);
		if (!query.exec(str))
		{
			return;
		}

		while (query.next())
		{
			QSqlRecord record = query.record();
			//查询到这个id
			std::string id = record.value(0).toString().toStdString();
			const std::string timeStamp = record.value(1).toString().toStdString();
			mapTimeStamp[id] = timeStamp;
		}
		for (auto& item : vecFriendInfo)
		{
			if (0 == mapTimeStamp.count(item.m_strId) || item.m_strImageTimestamp != mapTimeStamp[item.m_strId])
			{
				//TODO 发送消息到服务器，获取新头像
				protocol::getProfileImageJsonData tmpInfo;
				tmpInfo.m_strId = item.m_strId;
				TCPThread::get_mutable_instance().sendMessage(tmpInfo.generateJson());
			}
			else if (item.m_strImageTimestamp != mapTimeStamp[item.m_strId])
			{
				database::ProfilePictureDatabase profilePictureDatabase;
				profilePictureDatabase.updateFriendPictureTimestamp(item.m_strId.c_str(), item.m_strImageTimestamp.c_str());
			}
		}
	}

	void ChatWidgetManager::compareImageTimeStamp(std::vector<std::string> vecId)
	{
		auto dataBase = QSqlDatabase::addDatabase("QSQLITE", "sqlitecompareimage");
		QString dataName = QApplication::applicationDirPath() + "/data/chatinfo" + m_strUserId + ".db";
		dataBase.setDatabaseName(dataName);
		if (!dataBase.open())
		{
			qDebug() << "open database failed";
		}
		const QString str = "select id, timestamp from profileImage";
		std::unordered_set<std::string> setTimeStamp;
		QSqlQuery query(dataBase);
		if (!query.exec(str))
		{
			return;
		}

		while (query.next())
		{
			QSqlRecord record = query.record();
			//查询到这个id
			std::string id = record.value(0).toString().toStdString();
			setTimeStamp.insert(id);
		}
		for (auto& item : vecId)
		{
			if (0 == setTimeStamp.count(item))
			{
				//TODO 发送消息到服务器，获取新头像
				protocol::getProfileImageJsonData tmpInfo;
				tmpInfo.m_strId = item;
				TCPThread::get_mutable_instance().sendMessage(tmpInfo.generateJson());
			}
		}
	}

	std::vector<MyChatMessageInfo> ChatWidgetManager::getChatMessageAcordIdAtInit(QString strId)
	{
		//先看这个id的聊天记录有多少
		database::UserChatDatabase userChatDatabase(strId.toStdString().c_str(), "");
		int iMessageCount = userChatDatabase.queryChatRecordCountFromDB(strId);
		//如果大于10条就加载十条，小于十条就有多少加载多少
		int needLoadCount = (std::min)(10, iMessageCount);

		//获取聊天记录
		std::vector<MyChatMessageInfo> vecMyChatMessageInfo;

		if (0 == needLoadCount)
		{
			return vecMyChatMessageInfo;
		}
		//查询聊天记录的起始位置是聊天页面当前的数量,查询的数量就是needcount
		userChatDatabase.queryCertainCountChatRecordAcodId(strId, vecMyChatMessageInfo, needLoadCount, 0);
		return vecMyChatMessageInfo;
	}

	void ChatWidgetManager::handleSingleChatMessage(const QString& msg, const QString& id, const std::string timeStamp, const wechat::MyChatMessageQuickWid* chatWidgetPtr)
	{
		//通过网络将信息发送出去
		protocol::SingleChatMessageJsonData singleChatData;
		singleChatData.m_strSendUserId = m_strUserId.toStdString();
		singleChatData.m_strRecvUserId = (id).toStdString();
		singleChatData.m_strMessage = msg.toStdString();
		singleChatData.m_strTime = timeStamp;
		singleChatData.m_strSendName = m_strUserName.toStdString();
		std::string sendMessage = singleChatData.generateJson();
		TCPThread::get_mutable_instance().sendMessage(sendMessage);

		auto tablename = "chatrecord" + singleChatData.m_strRecvUserId;
		//查看数据库中这个表是否存在
		int totalCnt = chatWidgetPtr->getTotalRecordCount();
		if (database::DataBaseOperate::get_const_instance().isTableExist(QString::fromStdString(tablename)))
		{
			database::UserChatDatabase userChatDatabase(singleChatData.m_strRecvUserId.c_str(), "");
			//添加到数据库
			userChatDatabase.insertChatRecoed(totalCnt, QString::fromStdString(singleChatData.m_strMessage),
				QString::fromStdString(singleChatData.m_strTime), true, m_strUserName);
		}
		else
		{
			database::UserChatDatabase userChatDatabase(singleChatData.m_strRecvUserId.c_str(), "");
			//没有就创建这个表
			userChatDatabase.createChatTable();
			//添加到数据库
			userChatDatabase.insertChatRecoed(totalCnt, QString::fromStdString(singleChatData.m_strMessage),
				QString::fromStdString(singleChatData.m_strTime), true, m_strUserName);
		}
	}

	void ChatWidgetManager::initConnect()
	{
	}
}
