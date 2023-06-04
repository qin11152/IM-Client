#include "TCPThread.h"
#include "module/Log/Log.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "protocol/HeartPackageJsonData/HeartPackageJsonData.h"
#include "protocol/ImageMsgJsonData/ProfileImageMsgJsonData.h"

#include <QUuid>
#include <QFile>
#include <QBuffer>
#include <QPixmap>
#include <QApplication>


TCPThread::TCPThread(QObject *parent)
	:QThread(parent)
{
}

TCPThread::~TCPThread()
{
	unInit();
}

void TCPThread::unInit()
{
	disConnect();
	m_ptrTimerRecvHeartPackage->stop();
	m_ptrTimerKeepAlive->stop();
	delete m_ptrTimerRecvHeartPackage;
	delete m_ptrTimerKeepAlive;
	delete m_ptrTcpSocket;
}


void TCPThread::run()
{
	init();
	initConnect();
	//connectHost();
	exec();
}

void TCPThread::sendMessage(std::string message)
{
	if (!m_bIsConnected)
	{
		//m_bIsConnected = true;
		connectHost();
	}
	emit signalSendMsg(message);
}

void TCPThread::sendImageMsg(const QString& strBase64Image, const QString& ImageName, const QString& suffix, const QString& timeStamp)
{
	if (!m_bIsConnected)
	{
		//m_bIsConnected = true;
		connectHost();
	}
	emit signalSendImageMsg(strBase64Image, ImageName, suffix, timeStamp);
}

void TCPThread::sendJsonWithImage(const QString& strBase64Image, const QString& jsonStr)
{
	if (!m_bIsConnected)
	{
		connectHost();
	}
	emit signalSendJsonWithImage(strBase64Image, jsonStr);
}

void TCPThread::sendImage(const char* filePath, const char* jsonMsgOfImage)
{
	emit signalSendIma(filePath,jsonMsgOfImage);
}

void TCPThread::sendImage(const QByteArray& image, const char* jsonMsgOfImage)
{
	emit signalQtSendIma(image, jsonMsgOfImage);
}

void TCPThread::connectHost()
{
	emit signalConnect();
}

void TCPThread::initConnect()
{
	//有消息到来时，读取消息，并发送给客户端
	connect(m_ptrTcpSocket, &QTcpSocket::readyRead, this, &TCPThread::onSignalRecvMessage);
	connect(m_ptrTimerKeepAlive, &QTimer::timeout, this, &TCPThread::onSignalTimeoutSendHeartPackage);
	connect(this, &TCPThread::signalSendMsg, m_ptrTcpSocket, &MyTCPSocket::sendMsg);
	connect(this, &TCPThread::signalSendImageMsg, m_ptrTcpSocket, &MyTCPSocket::sendImageMsg);
	connect(m_ptrTimerRecvHeartPackage, &QTimer::timeout, this, &TCPThread::onSignalTimeoutNoHeartPackage);
	connect(this, &TCPThread::signalConnect, m_ptrTcpSocket, &MyTCPSocket::connectHost);
	connect(m_ptrTcpSocket, &MyTCPSocket::signalConnectResult, this, &TCPThread::onConnectResult);
	connect(this, &TCPThread::signalSendIma, m_ptrTcpSocket, &MyTCPSocket::sendImage);
	connect(this, &TCPThread::signalQtSendIma, m_ptrTcpSocket, &MyTCPSocket::sendQImage);
}

void TCPThread::disConnect()
{
	m_ptrTcpSocket->disconnectFromHost();
	m_bIsConnected = false;
}

void TCPThread::init()
{
	//创建socket
	m_ptrTcpSocket = new MyTCPSocket();
	m_ptrTimerKeepAlive = new QTimer();
	m_ptrTimerRecvHeartPackage = new QTimer();
	m_ptrTimerRecvHeartPackage->setInterval(kHeartTimeoutTime);
	m_ptrTimerRecvHeartPackage->start();
	m_ptrTimerKeepAlive->setInterval(kKeepAlivePackageTime);
	m_ptrTimerKeepAlive->start();
}

void TCPThread::onSignalTimeoutNoHeartPackage()
{
	emit signalTimeoutNoHeartPackage();
}

void TCPThread::onSignalTimeoutSendHeartPackage()
{
	//发送给服务器的心跳包
	protocol::HeartPackageJsonData heartPackage;
	std::string msg = heartPackage.generateJson();
	sendMessage(msg);
	if (m_ptrTimerKeepAlive != nullptr && m_ptrTimerKeepAlive->isActive())
	{
		m_ptrTimerKeepAlive->stop();
	}
	m_ptrTimerKeepAlive->setInterval(kKeepAlivePackageTime);
	m_ptrTimerKeepAlive->start();
}

void TCPThread::onSignalRecvMessage()
{
	QByteArray message = m_ptrTcpSocket->readAll();
	memcpy(m_msgBuffer + m_endPosOfBuffer, message.toStdString().c_str(), message.length());
	m_endPosOfBuffer += message.length();
	
	size_t pos = 0;
	
	//要读到固定的包头才行
	while (pos < m_endPosOfBuffer && memcmp(m_msgBuffer + pos, "&q*b", 4) != 0)
	{
		pos += 4;
	}
	memcpy(m_msgBuffer, m_msgBuffer + pos, m_endPosOfBuffer - pos);
	m_endPosOfBuffer -= pos;

	//当缓冲区的长度大于包头长度时就可以进入业务处理逻辑了
	while (m_endPosOfBuffer > sizeof(PackageHead))
	{
		char lengthStr[sizeof(PackageHead) + 1]{ 0 };
		memcpy(lengthStr, m_msgBuffer, sizeof(PackageHead));
		auto head = reinterpret_cast<PackageHead*>(lengthStr);
		size_t iMsgLength = head->length;

		if (0x0011 == head->cmdId)
		{

			//判断收到的数据是否大于包头的长度
			if (m_endPosOfBuffer - PackageLength >= iMsgLength)
			{
				std::string test(m_msgBuffer + PackageLength, iMsgLength);
				//因已取出一部分信息，要把大缓冲区的内容更新一下
				memcpy(m_msgBuffer, m_msgBuffer + iMsgLength + PackageLength, kMsgBufferLength - iMsgLength - PackageLength);
				//尾部标识也更新一下
				m_endPosOfBuffer -= (iMsgLength + PackageLength);
				onHandleMessage(test);
			}
			//不够长就退出，包头数据保存，下次在收到数据会插入尾部
			else
			{
				char buf[1024]{ 0 };
				while (m_endPosOfBuffer - PackageLength < iMsgLength)
				{
					int len = m_ptrTcpSocket->read(buf, 1024);
					m_endPosOfBuffer += len;
					memcpy(m_msgBuffer + m_endPosOfBuffer, buf, len);
					memset(buf, 0, 1024);
				}
				std::string test(m_msgBuffer + PackageLength, iMsgLength);
				//因已取出一部分信息，要把大缓冲区的内容更新一下
				memcpy(m_msgBuffer, m_msgBuffer + iMsgLength + PackageLength, kMsgBufferLength - iMsgLength - PackageLength);
				//尾部标识也更新一下
				m_endPosOfBuffer -= (iMsgLength + PackageLength);
				onHandleMessage(test);
			}
		}
		else if (0x0012 == head->cmdId)
		{
			//接受图片
			int needReadLength = iMsgLength;
			std::string tmpStr(m_msgBuffer + PackageLength, (m_endPosOfBuffer - PackageLength));
			needReadLength -= (m_endPosOfBuffer - PackageLength);
			memcpy(m_msgBuffer, m_msgBuffer + iMsgLength + PackageLength, kMsgBufferLength - iMsgLength - PackageLength);
			//尾部标识也更新一下
			m_endPosOfBuffer -= (iMsgLength + PackageLength);
			//打开文件并写入
			std::string fileName = "D:/test.jpg";
			std::ofstream file(fileName, std::ios::binary);
			file.write(tmpStr.c_str(), tmpStr.length());
			char recvBuf[1024]{ 0 };
			while (needReadLength > 0)
			{
				int num = m_ptrTcpSocket->read(recvBuf, 1024);
				needReadLength -= num;
				file.write(recvBuf, num);
				memset(recvBuf, 0, 1024);
				needReadLength -= num;
			}
			file.close();
		}
	}
}

void TCPThread::onHandleMessage(const std::string& recvMessage)
{
	ptree m_ptree;
	std::stringstream sstream(recvMessage);
#if defined(QT_DEBUG)
	qDebug() << recvMessage.c_str();
	_LOG(Logcxx::Level::DEBUG, recvMessage.c_str());
#endif
	read_json(sstream, m_ptree);
	int messageType = m_ptree.get<int>("type");
	switch (messageType)
	{
		case int(MessageType::RegisterResponse) :
		{
			emit signalRecvRegisterMessage(QString::fromStdString(recvMessage));
		}
		break;
		case int(MessageType::LoginResponse) :
		{
			emit signalRecvLoginResultMessage(QString::fromStdString(recvMessage));
		}
		break;
		case int(MessageType::SingleChat) :
		{
			emit signalRecvSingleChatMessage(QString::fromStdString(recvMessage));
		}
		break;
		case int(MessageType::GetFriendListReply) :
		{
			emit signalRecvFriendListMessage(QString::fromStdString(recvMessage));
		}
		break;
		case int(MessageType::AddFriendRequest) :
		{
			emit signalNewFriendRequest(QString::fromStdString(recvMessage));
		}
		break;
		case int(MessageType::AddFriendNotify) :
		{
			emit signalBecomeFriendNotify(QString::fromStdString(recvMessage));
		}
		break;
		case int(MessageType::ProfileImageMsg) : 
		{
			protocol::ProfileImageMsgJsonData profileImageMsgData(recvMessage);
			int iNeedSegment = profileImageMsgData.m_iSumIndex;
			if (m_mapImageUUIDAndSegment.count(profileImageMsgData.m_strUUID) && profileImageMsgData.m_iCurIndex - 1 != m_mapImageUUIDAndSegment[profileImageMsgData.m_strUUID])
			{
				//TODO 回复一个uuid发送失败的消息
				m_mapImageUUIDAndBase64.erase(profileImageMsgData.m_strUUID);
				m_mapImageUUIDAndSegment.erase(profileImageMsgData.m_strUUID);
			}
			m_mapImageUUIDAndBase64[profileImageMsgData.m_strUUID] += profileImageMsgData.m_strBase64Msg;
			m_mapImageUUIDAndSegment[profileImageMsgData.m_strUUID] = profileImageMsgData.m_iCurIndex;
			if (profileImageMsgData.m_iCurIndex == profileImageMsgData.m_iSumIndex)
			{
				//如果收到的片数到达了最后一个了
				// 先将之前的图片删除
				
				QString oldPath = "";
				DataBaseDelegate::Instance()->queryProfileImagePath(profileImageMsgData.m_strId.c_str(), oldPath);
				if (oldPath != " ")
				{
					QFile::remove(oldPath);
				}

				//将图片保存到本地，并将图片的路径保存到数据库中
				QString savePath = PublicDataManager::get_mutable_instance().getIdDirPath() + "/image/" + profileImageMsgData.m_strId.c_str() + "." + profileImageMsgData.m_strSuffix.c_str();

				//qt BASE64转图片
				QByteArray ba;
				ba.append(m_mapImageUUIDAndBase64[profileImageMsgData.m_strUUID].c_str());
				QByteArray bb = QByteArray::fromBase64(ba);
				
				QBuffer buffer(&bb);
				buffer.open(QIODevice::WriteOnly);
				QPixmap imageresult;
				bool resutl = imageresult.loadFromData(bb);
				if (savePath != "")
				{
					imageresult.save(savePath);
				}
				//TODO这里需要判断有没有这个id对应的数据
				//把这个新的路径存储在数据库中，并把新的路径更新到界面之中
				DataBaseDelegate::Instance()->updateProfilleImagePathAndTimeStamp(profileImageMsgData.m_strId.c_str(), savePath, profileImageMsgData.m_strTimeStamp.c_str());
				//更新头像的处理
				if (ProfileImageType::UpdateProfileImage == profileImageMsgData.m_eImageType)
				{
					emit signalProfileImageChanged(profileImageMsgData.m_strId.c_str(), savePath);
				}
				//服务器推送了一个添加你好友的人的头像
				else if(ProfileImageType::AddFriendProfileImage== profileImageMsgData.m_eImageType)
				{
					emit signalAddFriendProfileImage(profileImageMsgData.m_strId.c_str(),profileImageMsgData.m_strTimeStamp.c_str(), savePath);
				}
				m_mapImageUUIDAndBase64.erase(profileImageMsgData.m_strUUID);
				m_mapImageUUIDAndSegment.erase(profileImageMsgData.m_strUUID);
			}
		}
		break;
		case int(MessageType::StartGroupChatReply) :
		{
			emit signalStartGroupChatReply(QString::fromStdString(recvMessage));
		}
		break;
		default:
			break;
	}
}

void TCPThread::onConnectResult(bool bResult)
{
	if (bResult)
	{
		m_bIsConnected = true;
	}
	else
	{
		m_bIsConnected = false;
		emit signalConnectFailed();
	}
}
