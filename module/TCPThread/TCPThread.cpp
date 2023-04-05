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
	//����Ϣ����ʱ����ȡ��Ϣ�������͸��ͻ���
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
	//����socket
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
	//���͸���������������
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
	
	//Ҫ�����̶��İ�ͷ����
	while (pos < m_endPosOfBuffer && memcmp(m_msgBuffer + pos, "&q*b", 4) != 0)
	{
		pos += 4;
	}
	memcpy(m_msgBuffer, m_msgBuffer + pos, m_endPosOfBuffer - pos);
	m_endPosOfBuffer -= pos;

	//���������ĳ��ȴ��ڰ�ͷ����ʱ�Ϳ��Խ���ҵ�����߼���
	while (m_endPosOfBuffer > sizeof(PackageHead))
	{
		char lengthStr[sizeof(PackageHead) + 1]{ 0 };
		memcpy(lengthStr, m_msgBuffer, sizeof(PackageHead));
		auto head = reinterpret_cast<PackageHead*>(lengthStr);
		size_t iMsgLength = head->length;

		if (0x0011 == head->cmdId)
		{

			//�ж��յ��������Ƿ���ڰ�ͷ�ĳ���
			if (m_endPosOfBuffer - PackageLength >= iMsgLength)
			{
				std::string test(m_msgBuffer + PackageLength, iMsgLength);
				//����ȡ��һ������Ϣ��Ҫ�Ѵ󻺳��������ݸ���һ��
				memcpy(m_msgBuffer, m_msgBuffer + iMsgLength + PackageLength, kMsgBufferLength - iMsgLength - PackageLength);
				//β����ʶҲ����һ��
				m_endPosOfBuffer -= (iMsgLength + PackageLength);
				onHandleMessage(test);
			}
			//���������˳�����ͷ���ݱ��棬�´����յ����ݻ����β��
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
				//����ȡ��һ������Ϣ��Ҫ�Ѵ󻺳��������ݸ���һ��
				memcpy(m_msgBuffer, m_msgBuffer + iMsgLength + PackageLength, kMsgBufferLength - iMsgLength - PackageLength);
				//β����ʶҲ����һ��
				m_endPosOfBuffer -= (iMsgLength + PackageLength);
				onHandleMessage(test);
			}
		}
		else if (0x0012 == head->cmdId)
		{
			//����ͼƬ
			int needReadLength = iMsgLength;
			std::string tmpStr(m_msgBuffer + PackageLength, (m_endPosOfBuffer - PackageLength));
			needReadLength -= (m_endPosOfBuffer - PackageLength);
			memcpy(m_msgBuffer, m_msgBuffer + iMsgLength + PackageLength, kMsgBufferLength - iMsgLength - PackageLength);
			//β����ʶҲ����һ��
			m_endPosOfBuffer -= (iMsgLength + PackageLength);
			//���ļ���д��
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
				//TODO �ظ�һ��uuid����ʧ�ܵ���Ϣ
				m_mapImageUUIDAndBase64.erase(profileImageMsgData.m_strUUID);
				m_mapImageUUIDAndSegment.erase(profileImageMsgData.m_strUUID);
			}
			m_mapImageUUIDAndBase64[profileImageMsgData.m_strUUID] += profileImageMsgData.m_strBase64Msg;
			m_mapImageUUIDAndSegment[profileImageMsgData.m_strUUID] = profileImageMsgData.m_iCurIndex;
			if (profileImageMsgData.m_iCurIndex == profileImageMsgData.m_iSumIndex)
			{
				//����յ���Ƭ�����������һ����
				// �Ƚ�֮ǰ��ͼƬɾ��
				
				QString oldPath = "";
				DataBaseDelegate::Instance()->queryProfileImagePath(profileImageMsgData.m_strId.c_str(), oldPath);
				if (oldPath != " ")
				{
					QFile::remove(oldPath);
				}

				//��ͼƬ���浽���أ�����ͼƬ��·�����浽���ݿ���
				QString savePath = PublicDataManager::get_mutable_instance().getIdDirPath() + "/image/" + profileImageMsgData.m_strId.c_str() + "." + profileImageMsgData.m_strSuffix.c_str();

				//qt BASE64תͼƬ
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
				//TODO������Ҫ�ж���û�����id��Ӧ������
				//������µ�·���洢�����ݿ��У������µ�·�����µ�����֮��
				DataBaseDelegate::Instance()->updateProfilleImagePathAndTimeStamp(profileImageMsgData.m_strId.c_str(), savePath, profileImageMsgData.m_strTimeStamp.c_str());
				//����ͷ��Ĵ���
				if (ProfileImageType::UpdateProfileImage == profileImageMsgData.m_eImageType)
				{
					emit signalProfileImageChanged(profileImageMsgData.m_strId.c_str(), savePath);
				}
				//������������һ���������ѵ��˵�ͷ��
				else if(ProfileImageType::AddFriendProfileImage== profileImageMsgData.m_eImageType)
				{
					emit signalAddFriendProfileImage(profileImageMsgData.m_strId.c_str(), savePath);
				}
				m_mapImageUUIDAndBase64.erase(profileImageMsgData.m_strUUID);
				m_mapImageUUIDAndSegment.erase(profileImageMsgData.m_strUUID);
			}
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
