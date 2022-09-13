#include "TCPConnect.h"
#include "protocol/HeartPackageJsonData/HeartPackageJsonData.h"
#include "protocol/ImageMsgJsonData/ProfileImageMsgJsonData.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "module/Log/Log.h"
#include <QUuid>
#include <QApplication>

using SingletonPtr = std::shared_ptr<TCPConnect>;
//��ʼ����̬��Ա����
SingletonPtr TCPConnect::m_SingletonPtr = nullptr;

std::mutex TCPConnect::m_mutex;

SingletonPtr TCPConnect::Instance()
{
    //˫�ر�������һ���пգ��������ɶ��
    if (m_SingletonPtr == nullptr)
    {
        std::lock_guard<std::mutex> lck(m_mutex);
        //�������пգ�������߳����ɶ��ʵ��
        if (m_SingletonPtr == nullptr)
        {
            m_SingletonPtr = std::shared_ptr<TCPConnect>(new TCPConnect);
        }
    }
    //����ָ��
    return m_SingletonPtr;
}

void TCPConnect::sendMessage(std::string message)
{
    if (!m_bIsConnected)
    {
        m_bIsConnected = true;
        connectHost();
    }
    //utf8����
    int length = message.length();
    std::string msgLength = std::to_string(length);
    if (msgLength.length() < 8)
    {
        std::string tmpStr(8 - msgLength.length(), '0');
        msgLength = tmpStr+msgLength;
    }
    std::string sendMsg = msgLength + message;
    int cnt=m_ptrTcpSocket->write(sendMsg.c_str(),length+8);
    printf("write %d byte,message is:%s\n", cnt,sendMsg.c_str());
}

void TCPConnect::sendLength(LengthInfo& l, int length)
{
    if (m_bIsConnected == false)
    {
        connectHost();
    }
    int cnt = m_ptrTcpSocket->write((const char*)(&l), length);
    //printf("write %d byte\n", cnt);
}

//����base64�����ͼƬ����
void TCPConnect::sendImageMsg(QString& strBase64Image, const QString& ImageName)
{
    //����uuid�������ͼƬ��Ψһ��
    QString strUUID = QUuid::createUuid().toString();
    //���ͼƬ��Ϣ�ĳ��ȴ�����Ҫ��Ƭ�ĳ��ȣ����Ƭ����
    int iNeedSlice = strBase64Image.length() / kSegmentLength;
    //���������������Ҫ��Ƭ��+1
    if (0 != strBase64Image.length() % kSegmentLength)
    {
        iNeedSlice++;
    }
    for (int i = 0; i < iNeedSlice; ++i)
    {
        ProfileImageMsgJsonData tmpImageData;
        tmpImageData.m_strId = PublicDataManager::get_mutable_instance().getMyId().toStdString();
        tmpImageData.m_strImageName = ImageName.toStdString();
        tmpImageData.m_strUUID = strUUID.toStdString();
        tmpImageData.m_iCurIndex = i + 1;
        tmpImageData.m_iSumIndex = iNeedSlice;
        if (i == iNeedSlice - 1)
        {
            tmpImageData.m_strBase64Msg = strBase64Image.mid(i * 9000, strBase64Image.length() - i * 9000).toStdString();
        }
        else
        {
            tmpImageData.m_strBase64Msg = strBase64Image.mid(i * 9000, 9000).toStdString();
        }
        sendMessage(tmpImageData.generateJson());
        //����һ���¼�ѭ���е��¼�
        qApp->processEvents();
    }
}

void TCPConnect::connectHost()
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

void TCPConnect::initConnect()
{
    //����Ϣ����ʱ����ȡ��Ϣ�������͸��ͻ���
    connect(m_ptrTcpSocket, &QTcpSocket::readyRead, this, &TCPConnect::onSignalRecvMessage);
    connect(m_ptrTimerKeepAlive, &QTimer::timeout, this, &TCPConnect::onSignalTimeoutSendHeartPackage);
    connect(m_ptrTimerRecvHeartPackage, &QTimer::timeout, this, &TCPConnect::onSignalTimeoutNoHeartPackage);
}

void TCPConnect::disConnect()
{
    m_ptrTcpSocket->disconnectFromHost();
    m_bIsConnected = false;
}

void TCPConnect::init()
{
    //����socket
    m_ptrTcpSocket = new QTcpSocket();
    m_ptrTimerKeepAlive = new QTimer();
    m_ptrTimerKeepAlive->setInterval(kKeepAlivePackageTime);
    m_ptrTimerKeepAlive->start();
    m_ptrTimerRecvHeartPackage = new QTimer();
    m_ptrTimerRecvHeartPackage->setInterval(kHeartTimeoutTime);
    m_ptrTimerRecvHeartPackage->start();
}

void TCPConnect::onSignalTimeoutNoHeartPackage()
{
    emit signalTimeoutNoHeartPackage();
}

void TCPConnect::onSignalTimeoutSendHeartPackage()
{
    //���͸���������������
    HeartPackageJsonData heartPackage;
    std::string msg=heartPackage.generateJson();
    sendMessage(msg);
    if (m_ptrTimerKeepAlive != nullptr && m_ptrTimerKeepAlive->isActive())
    {
        m_ptrTimerKeepAlive->stop();
    }
    m_ptrTimerKeepAlive->setInterval(kKeepAlivePackageTime);
    m_ptrTimerKeepAlive->start();
}

void TCPConnect::onSignalRecvMessage()
{
    QByteArray message = m_ptrTcpSocket->readAll();
    memcpy(m_msgBuffer + m_endPosOfBuffer, message.toStdString().c_str(), message.length());
    m_endPosOfBuffer += message.length();
    //���������ĳ��ȴ��ڰ�ͷ����ʱ�Ϳ��Խ���ҵ�����߼���
    while (m_endPosOfBuffer>PackageLength)
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
    /*if (m_ptrTimerKeepAlive != nullptr && m_ptrTimerKeepAlive->isActive())
    {
        m_ptrTimerKeepAlive->stop();
    }
    m_ptrTimerKeepAlive->setInterval(kKeepAlivePackageTime);
    m_ptrTimerKeepAlive->start();
    if (m_ptrTimerRecvHeartPackage != nullptr && m_ptrTimerRecvHeartPackage->isActive())
    {
        m_ptrTimerRecvHeartPackage->stop();
    }
    m_ptrTimerRecvHeartPackage->setInterval(kHeartTimeoutTime);
    m_ptrTimerRecvHeartPackage->start();
    */
}

void TCPConnect::onHandleMessage(const std::string& recvMessage)
{
    printf("recv msg:%s\n", recvMessage.c_str());
    ptree m_ptree;
    std::stringstream sstream(recvMessage);
    read_json(sstream, m_ptree);
    int messageType = m_ptree.get<int>("type");
    switch (messageType)
    {
        case int(MessageType::RegisterResponse):
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
        case int(MessageType::AddFriendRequest):
        {
            emit signalNewFriendRequest(QString::fromStdString(recvMessage));
        }
        break;
        case int(MessageType::AddFriendNotify):
        {
            emit signalBecomeFriendNotify(QString::fromStdString(recvMessage));
        }
        break;
        default:
            break;
    }
}

TCPConnect::TCPConnect(QObject *parent)
    : QObject(parent)
{
    init();
    //connectHost();
    initConnect();
}

TCPConnect::~TCPConnect()
{
    m_ptrTcpSocket->close();
    delete m_ptrTcpSocket;
    m_ptrTcpSocket = nullptr;
    if (m_ptrTimerKeepAlive != nullptr && m_ptrTimerKeepAlive->isActive())
    {
        m_ptrTimerKeepAlive->stop();
    }
    delete m_ptrTimerKeepAlive;
    m_ptrTimerKeepAlive = nullptr;
    if (m_ptrTimerRecvHeartPackage != nullptr && m_ptrTimerRecvHeartPackage->isActive())
    {
        m_ptrTimerRecvHeartPackage->stop();
    }
    delete m_ptrTimerRecvHeartPackage;
    m_ptrTimerRecvHeartPackage= nullptr;
}
