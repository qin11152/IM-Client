#include "TCPThread.h"
#include "protocol/HeartPackageJsonData/HeartPackageJsonData.h"
#include "protocol/ImageMsgJsonData/ProfileImageMsgJsonData.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include <QUuid>

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
    connectHost();
    exec();
    unInit();
}

void TCPThread::sendMessage(std::string message)
{
    if (!m_bIsConnected)
    {
        m_bIsConnected = true;
        connectHost();
    }
    emit signalSendMsg(message);
}

void TCPThread::sendImageMsg(QString& strBase64Image, const QString& ImageName)
{
    if (!m_bIsConnected)
    {
        m_bIsConnected = true;
        connectHost();
    }
    emit signalSendImageMsg(strBase64Image, ImageName);
}

void TCPThread::connectHost()
{
    m_ptrTcpSocket->connectToHost(m_strIp, m_iPort);
    //������Ӳ��ϣ��ͷ����źŸ��߿ͻ���
    if (!m_ptrTcpSocket->waitForConnected(7000))
    {
        qDebug() << QString::fromLocal8Bit("�޷����ӷ�����");
        emit signalConnectFailed();
        return;
    }
    m_bIsConnected = true;
    qDebug() << QString::fromLocal8Bit("���ӷ������ɹ�");
}

void TCPThread::initConnect()
{
    //����Ϣ����ʱ����ȡ��Ϣ�������͸��ͻ���
    connect(m_ptrTcpSocket, &QTcpSocket::readyRead, this, &TCPThread::onSignalRecvMessage);
    connect(m_ptrTimerKeepAlive, &QTimer::timeout, this, &TCPThread::onSignalTimeoutSendHeartPackage);
    connect(m_ptrTimerRecvHeartPackage, &QTimer::timeout, this, &TCPThread::onSignalTimeoutNoHeartPackage);
    connect(this, &TCPThread::signalSendMsg, m_ptrTcpSocket, &MyTCPSocket::sendMsg);
    connect(this, &TCPThread::signalSendImageMsg, m_ptrTcpSocket, &MyTCPSocket::sendImageMsg);
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
        default:
            break;
    }
}
