#include "ChatWidgetManager.h"
#include "module/Log/Log.h"
#include "module/TCPThread/TCPThread.h"
#include "module/FileManager/FileManager.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "protocol/GetFriendListJsonData/GetFriendListJsonData.h"
#include "protocol/InitialRequestJsonData/InitialRequestJsonData.h"
#include "protocol/GetFriendListReplyData/GetFriendListReplyData.h"
#include "protocol/AddFriendNotifyJsonData/AddFriendNotifyJsonData.h"
#include "protocol/getProfileImageJsonData/getProfileImageJsonData.h"
#include "protocol/AddFriendResponseJsonData/AddFriendResponseJsonData.h"
#include "protocol/AddFriendRequestJsonData/AddFriendRequestJsonData.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QApplication>


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
		_LOG(Logcxx::Level::INFO, "friend id:%s;name:%s��", item.m_strFriendId.c_str(), item.m_strFriendName.c_str());
#endif
		//��ӵ�qmlҳ��listmodel��
		QString strShouZiMu = Base::PinYin::convertToPinYin(QString::fromStdString(item.m_strFriendName)).toUpper().mid(0, 1);
		MyFriendInfoWithFirstC tmpFriendInfo;
		tmpFriendInfo.m_strFirstChacter = strShouZiMu.toStdString();
		tmpFriendInfo.m_strId = item.m_strFriendId;
		tmpFriendInfo.m_strName = item.m_strFriendName;
		tmpFriendInfo.m_strImageTimestamp = item.m_strFriendImageTimeStamp;
		//�����ݿ��ȡͷ��·��
		QString strImage = "";
		DataBaseDelegate::Instance()->queryProfileImagePath(item.m_strFriendId.c_str(), strImage);
		//������ݿ���û��ͷ��·������ʹ��Ĭ��ͷ��
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
	//���͸�������
	TCPThread::get_mutable_instance().sendMessage(addFriendResponseJsonData.generateJson());
	//�޸�ע����е�״̬Ϊtrue
	DataBaseDelegate::Instance()->updateFriendRequestStateAcordId(friendId);
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
	//֪ͨ����ı䣬�ϴ�����Ľ������һ���µĺ�����
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
	//д�����ݿ�
	const protocol::AddFriendRequestJsonData addFriendRequestData(msg.toStdString().c_str());
	const char* name = addFriendRequestData.m_strName.c_str();
	const char* friendId = addFriendRequestData.m_strFriendId.c_str();
	const char* verifyMsg = addFriendRequestData.m_strVerifyMsg.c_str();
	//��Ӻ��ѵĽ������һ���µ�
	DataBaseDelegate::Instance()->insertAddFriendRequest(friendId, name, verifyMsg);
	QMetaObject::invokeMethod(m_ptrAddFriendQMLRoot, "insertNewAddFriendRequest",
							  Q_ARG(QVariant, Base::PinYin::convertToPinYin(name).mid(0, 1)), Q_ARG(QVariant, name),
							  Q_ARG(QVariant, verifyMsg), Q_ARG(QVariant, false));
}

void ChatWidgetManager::onSignalUpdateLastChat()
{
	//�����߳�ȥ����
	if (!m_ptrDBOperateThread)
	{
		m_ptrDBOperateThread = new DatabaseOperateThread();
		m_ptrDBOperateThread->init();
	}

	QStringList tmpOrder;
	onSignalGetModelOrder(tmpOrder);
	setLastChatList(tmpOrder);

	//���ò�������Ϊ�����ϴ��������ݿ�
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
}

void ChatWidgetManager::initDBOperateThread()
{
	m_ptrDBOperateThread=new DatabaseOperateThread(nullptr);
	//Ϊ���߳��趨id����֤���������ݿ���ȷ
	m_ptrDBOperateThread->setCurUserId(m_strUserId);
}

//�鿴һЩ������ļ��к��ļ��Ƿ����
void ChatWidgetManager::initDirAndFile()
{
	//��ȡ��ǰ��������·��
	QString strAppPath = QCoreApplication::applicationDirPath();
	
	//�鿴data�ļ���
	QString strDataPath = strAppPath + "/data";
	if (!Base::fileoperate::FileManager::get_mutable_instance().checkDirExist(strDataPath))
	{
		Base::fileoperate::FileManager::get_mutable_instance().createDir(strDataPath);
	}

	//�鿴��Ӧid���ļ����Ƿ����
	QString idPath = strDataPath + "/" + m_strUserId;
	if (!Base::fileoperate::FileManager::get_mutable_instance().checkDirExist(idPath))
	{
		Base::fileoperate::FileManager::get_mutable_instance().createDir(idPath);
	}
	//���Լ���id��Ӧ���ļ���·����������
	PublicDataManager::get_mutable_instance().setIdDirPath(idPath);
	
	//�鿴image�ļ���
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

//����������ͳ�ʼ����Ϣ����֪��id����
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
	//map�ṹ�壬�洢�ϴιر�ʱ�����б��е�˳��
	DataBaseDelegate::Instance()->queryLastChatListFromDB(vecLastChatFriend);
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
		//��ѯ�����id
		std::string id = record.value(0).toString().toStdString();
		const std::string timeStamp = record.value(1).toString().toStdString();
		mapTimeStamp[id] = timeStamp;
	}
	for (auto& item : vecFriendInfo)
	{
		if (0 == mapTimeStamp.count(item.m_strId) || item.m_strImageTimestamp != mapTimeStamp[item.m_strId])
		{
			//TODO ������Ϣ������������ȡ��ͷ��
			protocol::getProfileImageJsonData tmpInfo;
			tmpInfo.m_strId = item.m_strId;
			TCPThread::get_mutable_instance().sendMessage(tmpInfo.generateJson());
		}
	}
}

std::vector<MyChatMessageInfo> ChatWidgetManager::getChatMessageAcordIdAtInit(QString strId)
{
	//�ȿ����id�������¼�ж���
	int iMessageCount = DataBaseDelegate::Instance()->getChatRecordCountFromDB(strId);
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

void ChatWidgetManager::initConnect()
{
}

