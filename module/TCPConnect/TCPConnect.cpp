#include "TCPConnect.h"
#include "protocol/HeartPackageJsonData/HeartPackageJsonData.h"
#include "protocol/ImageMsgJsonData/ProfileImageMsgJsonData.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "module/Log/Log.h"
#include <QUuid>
#include <QApplication>

using SingletonPtr = std::shared_ptr<TCPConnect>;
//初始化静态成员函数
SingletonPtr TCPConnect::m_SingletonPtr = nullptr;

std::mutex TCPConnect::m_mutex;

SingletonPtr TCPConnect::Instance()
{
    //双重保护，第一重判空，避免生成多个
    if (m_SingletonPtr == nullptr)
    {
        std::lock_guard<std::mutex> lck(m_mutex);
        //加锁后判空，避免多线程生成多个实例
        if (m_SingletonPtr == nullptr)
        {
            m_SingletonPtr = std::shared_ptr<TCPConnect>(new TCPConnect);
        }
    }
    //返回指针
    return m_SingletonPtr;
}

void TCPConnect::sendMessage(std::string message)
{
    if (!m_bIsConnected)
    {
        m_bIsConnected = true;
        connectHost();
    }
    //utf8编码
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

//发送base64编码的图片方法
void TCPConnect::sendImageMsg(QString& strBase64Image, const QString& ImageName)
{
    //生成uuid表明这个图片的唯一性
    QString strUUID = QUuid::createUuid().toString();
    //如果图片信息的长度大于需要分片的长度，则分片发送
    int iNeedSlice = strBase64Image.length() / kSegmentLength;
    //如果不是整除，需要的片数+1
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
        //处理一下事件循环中的事件
        qApp->processEvents();
    }
}

void TCPConnect::connectHost()
{
    m_ptrTcpSocket->connectToHost(m_strIp, m_iPort);
    //如果连接不上，就发出信号告诉客户端
    if (!m_ptrTcpSocket->waitForConnected(7000))
    {
        qDebug() << QString::fromLocal8Bit("无法连接服务器");
        emit signalConnectFailed();
        return;
    }
    m_bIsConnected = true;
    qDebug() << QString::fromLocal8Bit("连接服务器成功");
}

void TCPConnect::initConnect()
{
    //有消息到来时，读取消息，并发送给客户端
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
    //创建socket
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
    //发送给服务器的心跳包
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
    //当缓冲区的长度大于包头长度时就可以进入业务处理逻辑了
    while (m_endPosOfBuffer>PackageLength)
    {
        char msgLength[PackageLength + 1]{ 0 };
        memcpy(msgLength, m_msgBuffer, PackageLength);
        int iMsgLength = atoi(msgLength);
        //判断收到的数据是否大于包头的长度
        if (m_endPosOfBuffer - PackageLength >= iMsgLength)
        {
            char recvMessage[1024]{ 0 };
            memcpy(recvMessage, m_msgBuffer + PackageLength, iMsgLength);
            onHandleMessage(recvMessage);
            //把处理完的数据覆盖掉
            memcpy(m_msgBuffer, m_msgBuffer + PackageLength + iMsgLength, kMsgBufferLength - (PackageLength + iMsgLength));
            //更新一下尾部的位置
            m_endPosOfBuffer -= PackageLength + iMsgLength;
        }
        //不够长就退出，包头数据保存，下次在收到数据会插入尾部
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
