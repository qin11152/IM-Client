#include "MyTCPSocket.h"
#include "protocol/ImageMsgJsonData/ProfileImageMsgJsonData.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include <QUuid>

MyTCPSocket::MyTCPSocket(QObject *parent)
    : QTcpSocket(parent)
{}

MyTCPSocket::~MyTCPSocket()
{
}

void MyTCPSocket::sendMsg(const std::string& msg)
{
    //utf8����
    int length = msg.length();
    std::string msgLength = std::to_string(length);
    if (msgLength.length() < 8)
    {
        std::string tmpStr(8 - msgLength.length(), '0');
        msgLength = tmpStr + msgLength;
    }
    std::string sendMsg = msgLength + msg;
    write(sendMsg.c_str(), length + 8);
}

void MyTCPSocket::sendImageMsg(const QString& strBase64Image, const QString& ImageName, const QString& suffix, const QString& timeStamp)
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
        tmpImageData.m_strImageName = ImageName.toStdString();
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
        sendMsg(tmpImageData.generateJson());
    }
}

void MyTCPSocket::connectHost()
{
    connectToHost(kHostIp, kHostPort);
    //������Ӳ��ϣ��ͷ����źŸ��߿ͻ���
    if (!waitForConnected(7000))
    {
        emit signalConnectResult(false);
        return;
    }
    emit signalConnectResult(true);
}

void MyTCPSocket::initConnect()
{
}


