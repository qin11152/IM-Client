#include "TCPOperateInterface.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "module/DataBaseDelegate/DatabaseOperateNeededFile.h"
#include "protocol/ImageMsgJsonData/ProfileImageMsgJsonData.h"
#include "protocol/HeartPackageJsonData/HeartPackageJsonData.h"
#include <qbuffer.h>
#include <quuid.h>

namespace module
{
    struct PackageHead
    {
        char flagBit[4]{ 0 };
        unsigned int length;
        unsigned char cmdId;
    };
    constexpr size_t PackageLength = sizeof(PackageHead);

    TCPOperate::TCPOperate(QObject* parent)
        : QObject(parent)
    {
    }

    TCPOperate::~TCPOperate()
    {
        //if (m_timerKeepAlive)
        //{
        //    m_timerKeepAlive->stop();
        //}
        //delete m_timerKeepAlive;
        //m_timerKeepAlive = nullptr;
        //delete m_ptrTCPSocket;
    }

    void TCPOperate::unInit()
    {
        disconnect();
        delete m_ptrTCPSocket;
        m_ptrTCPSocket = nullptr;
        if (m_timerKeepAlive)
        {
            m_timerKeepAlive->stop();
        }
        delete m_timerKeepAlive;
        m_timerKeepAlive = nullptr;
    }

    void TCPOperate::sendMessageInternal(std::string message)
    {
        if (!m_bConnectResult)
        {
            connectHost();
        }
        int length = message.length();
        PackageHead head;
        memcpy(head.flagBit, "&q*b", 4);
        head.length = length;
        head.cmdId = 0x0011;
        char tmp[sizeof(PackageHead)]{ 0 };
        memcpy(tmp, (char*)&head, sizeof(PackageHead));
        std::string strHead((char*)&head, sizeof(PackageHead));
        std::string sendMsg = strHead + message;
        int num = m_ptrTCPSocket->write(sendMsg.c_str(), length + sizeof(PackageHead));
#if defined(Test)
        qDebug() << "send length:" << num << ",msg length:" << length;
#endif
    }

    void TCPOperate::sendImageInternel(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp)
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
            protocol::ProfileImageMsgJsonData tmpImageData;
            tmpImageData.m_strId = PublicDataManager::get_mutable_instance().getMyId().toStdString();
            tmpImageData.m_strImageName = imageName.toStdString();
            tmpImageData.m_strUUID = strUUID.toStdString();
            tmpImageData.m_iCurIndex = i + 1;
            tmpImageData.m_iSumIndex = iNeedSlice;
            tmpImageData.m_strSuffix = suffix.toStdString();
            tmpImageData.m_strTimeStamp = timeStamp.toStdString();
            if (i == iNeedSlice - 1)
            {
                tmpImageData.m_strBase64Msg = strBase64Image.mid(i * kSegmentLength, strBase64Image.length() - i * 9000).toStdString();
            }
            else
            {
                tmpImageData.m_strBase64Msg = strBase64Image.mid(i * kSegmentLength, kSegmentLength).toStdString();
            }
            sendMessageInternal(tmpImageData.generateJson());
        }
    }

    void TCPOperate::onSignalTimeoutSendHeartPackage()
    {
        //���͸���������������
        protocol::HeartPackageJsonData heartPackage;
        std::string msg = heartPackage.generateJson();
        sendMessage(msg);
        m_timerKeepAlive->stop();
        m_timerKeepAlive->setInterval(kKeepAlivePackageTime);
        m_timerKeepAlive->start();
    }

    void TCPOperate::onSignalTimeoutNoHeartPackage()
    {
        emit signalTimeoutNoHeartPackage();
    }

    void TCPOperate::onSignalRecvMessage()
    {
        QByteArray message = m_ptrTCPSocket->readAll();
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
                        int len = m_ptrTCPSocket->read(buf, 1024);
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
                    int num = m_ptrTCPSocket->read(recvBuf, 1024);
                    needReadLength -= num;
                    file.write(recvBuf, num);
                    memset(recvBuf, 0, 1024);
                    needReadLength -= num;
                }
                file.close();
            }
        }
    }

    void TCPOperate::connectHost()
    {
        if (m_bConnectResult)
        {
            return;
        }
        m_ptrTCPSocket->connectToHost(kHostIp, kHostPort);
        //������Ӳ��ϣ��ͷ����źŸ��߿ͻ���
        if (!m_ptrTCPSocket->waitForConnected(7000))
        {
            m_bConnectResult = false;
            return;
        }
        m_timerKeepAlive->setInterval(kKeepAlivePackageTime);
        m_timerKeepAlive->start();
        m_bConnectResult = true;
    }

    void TCPOperate::disconnectHost()
    {
        m_ptrTCPSocket->disconnectFromHost();
        m_bConnectResult = false;
    }

    void TCPOperate::onHandleMessage(const std::string& recvMessage)
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
                    database::ProfilePictureDatabase profilePictureDatabase;
                    profilePictureDatabase.queryProfilePicturePath(profileImageMsgData.m_strId.c_str(), oldPath);
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
                    profilePictureDatabase.updateProfillePicturePathAndTimeStamp(profileImageMsgData.m_strId.c_str(), savePath, profileImageMsgData.m_strTimeStamp.c_str());
                    //����ͷ��Ĵ���
                    if (ProfileImageType::UpdateProfileImage == profileImageMsgData.m_eImageType)
                    {
                        emit signalProfileImageChanged(profileImageMsgData.m_strId.c_str(), savePath);
                    }
                    //������������һ���������ѵ��˵�ͷ��
                    else if (ProfileImageType::AddFriendProfileImage == profileImageMsgData.m_eImageType)
                    {
                        emit signalAddFriendProfileImage(profileImageMsgData.m_strId.c_str(), profileImageMsgData.m_strTimeStamp.c_str(), savePath);
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

    void TCPOperate::init()
    {
        qDebug() << "sub thread Id" << GetCurrentThreadId();
        m_timerKeepAlive = new QTimer();
        connect(m_timerKeepAlive, &QTimer::timeout, this, &TCPOperate::onSignalTimeoutSendHeartPackage);
        m_ptrTCPSocket = new QTcpSocket();
        connect(this, &TCPOperate::sendMessage, this, &TCPOperate::sendMessageInternal);
        //����Ϣ����ʱ����ȡ��Ϣ�����׸������
        connect(m_ptrTCPSocket, &QTcpSocket::readyRead, this, &TCPOperate::onSignalRecvMessage);
        connectHost();
    }

    TCPOperateInterface::TCPOperateInterface()
        :m_ptrTCPOperate(new TCPOperate(nullptr))
    {
        m_ptrTCPOperate->moveToThread(&m_operateThread);
        connect(&m_operateThread, &QThread::started, m_ptrTCPOperate, &TCPOperate::init);
        connect(&m_operateThread, &QThread::finished, m_ptrTCPOperate, &TCPOperate::unInit);
        connect(this,&TCPOperateInterface::signalSendImageMsg, m_ptrTCPOperate, &TCPOperate::sendImageInternel);
        connect(this,&TCPOperateInterface::signalSendMessage, m_ptrTCPOperate, &TCPOperate::sendMessageInternal);
        m_operateThread.start();
    }

    TCPOperateInterface::~TCPOperateInterface()
    {
        m_operateThread.exit();
        m_operateThread.wait();
        delete m_ptrTCPOperate;
        m_ptrTCPOperate = nullptr;
    }
    void TCPOperateInterface::sendMessageExternalInterface(std::string message)
    {
        emit signalSendMessage(message);
    }
    void TCPOperateInterface::sendImageMsgExternalInterface(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp)
    {
        emit signalSendImageMsg(strBase64Image, imageName, suffix, timeStamp);
    }
}