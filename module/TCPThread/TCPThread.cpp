#include "TCPThread.h"
#include "protocol/HeartPackageJsonData/HeartPackageJsonData.h"
#include "protocol/ImageMsgJsonData/ProfileImageMsgJsonData.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include <QUuid>
#include <QApplication>

TCPThread::TCPThread(QObject *parent)
    :QThread(parent)
{
}

TCPThread::~TCPThread()
{
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
    unInit();
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
    HeartPackageJsonData heartPackage;
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
    //���������ĳ��ȴ��ڰ�ͷ����ʱ�Ϳ��Խ���ҵ�����߼���
    while (m_endPosOfBuffer > PackageLength)
    {
        char msgLength[PackageLength + 1]{ 0 };
        memcpy(msgLength, m_msgBuffer, PackageLength);
        int iMsgLength = atoi(msgLength);
        //�ж��յ��������Ƿ���ڰ�ͷ�ĳ���
        if (m_endPosOfBuffer - PackageLength >= iMsgLength)
        {
            char recvMessage[1024]{ 0 };
            memcpy(recvMessage, m_msgBuffer + PackageLength, iMsgLength);
            onHandleMessage(recvMessage);
            //�Ѵ���������ݸ��ǵ�
            memcpy(m_msgBuffer, m_msgBuffer + PackageLength + iMsgLength, kMsgBufferLength - (PackageLength + iMsgLength));
            //����һ��β����λ��
            m_endPosOfBuffer -= PackageLength + iMsgLength;
        }
        //���������˳�����ͷ���ݱ��棬�´����յ����ݻ����β��
        else
        {
            break;
        }
    }
}

void TCPThread::onHandleMessage(const std::string& recvMessage)
{
    ptree m_ptree;
    std::stringstream sstream(recvMessage);
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
            ProfileImageMsgJsonData profileImageMsgData(recvMessage);
            int iNeedSegment = profileImageMsgData.m_iSumIndex;
            if (m_mapImageUUIDAndSegment.count(profileImageMsgData.m_strUUID) && profileImageMsgData.m_iCurIndex - 1 != m_mapImageUUIDAndSegment[profileImageMsgData.m_strUUID])
            {
                //TODO �ظ�һ��uuid����ʧ�ܵ���Ϣ
                m_mapImageUUIDAndBase64.erase(profileImageMsgData.m_strUUID);
                m_mapImageUUIDAndSegment.erase(profileImageMsgData.m_strUUID);
            }
            m_mapImageUUIDAndBase64[profileImageMsgData.m_strUUID] += profileImageMsgData.m_strBase64Msg;
            if (profileImageMsgData.m_iCurIndex == profileImageMsgData.m_iSumIndex)
            {
                //����յ���Ƭ�����������һ����
                //TODO ��ͼƬ���浽���أ�����ͼƬ��·�����浽���ݿ���
                std::string curPath = QApplication::applicationDirPath().toStdString();
                curPath += "/data/profileImage/" + profileImageMsgData.m_strId + "." + profileImageMsgData.m_strSuffix;
                std::fstream out(curPath, std::ios::out);
                if (out.is_open())
                {
                    out << m_mapImageUUIDAndBase64[profileImageMsgData.m_strUUID];
                    out.close();
                }
                else {
                }
                //TODO������µ�·���洢�����ݿ��У������µ�·�����µ�����֮��
                //MysqlQuery::Instance()->updateImagePathAcordId(profileImageMsgData.m_strId, curPath);
                m_mapImageUUIDAndBase64.erase(profileImageMsgData.m_strUUID);
                m_mapImageUUIDAndSegment.erase(profileImageMsgData.m_strUUID);
                //TODO �ظ�һ�����ͳɹ�����Ϣ
            }
            m_mapImageUUIDAndSegment[profileImageMsgData.m_strUUID] = profileImageMsgData.m_iCurIndex;
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
