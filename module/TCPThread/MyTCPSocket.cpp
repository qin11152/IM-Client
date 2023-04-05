#include "MyTCPSocket.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "protocol/ImageMsgJsonData/ProfileImageMsgJsonData.h"

#include <QUuid>

MyTCPSocket::MyTCPSocket(QObject *parent)
    : QTcpSocket(parent)
{}

MyTCPSocket::~MyTCPSocket()
{
}

void MyTCPSocket::sendMsg(const std::string& msg)
{
    //utf8编码
    int length = msg.length();
    PackageHead head;
    memcpy(head.flagBit, "&q*b", 4);
    head.length = length;
    head.cmdId = 0x0011;
    char tmp[sizeof(PackageHead)]{ 0 };
    memcpy(tmp, (char*)&head, sizeof(PackageHead));
    std::string strHead((char*)&head, sizeof(PackageHead));
    std::string sendMsg = strHead + msg;
    int num = write(sendMsg.c_str(), length + sizeof(PackageHead));
#if defined(Test)
    qDebug() << "send length:" << num << ",msg length:" << length;
#endif
}

void MyTCPSocket::sendImageMsg(const QString& strBase64Image, const QString& ImageName, const QString& suffix, const QString& timeStamp)
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

void MyTCPSocket::sendImage(const QString& filePath, const QString& jsonMsgOfImage)
{
    PackageHead head;

    long size = 0;
    FILE* fp = fopen(filePath.toStdString().c_str(), "r");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    memcpy(head.flagBit, "&q*b", 4);
    if (jsonMsgOfImage.length() > 0)
    {
        head.length =  jsonMsgOfImage.length();
    }
    head.cmdId = 0x0012;

    std::string strHead((char*)&head, sizeof(PackageHead));
    int sends=write(strHead.c_str(), PackageLength);

    if (jsonMsgOfImage.length() > 0)
    {
        sends += write(jsonMsgOfImage.toStdString().c_str(), jsonMsgOfImage.length());
    }

    //读取图片文件并发送出去
    char* imageBuf=new char[size];
    fread(imageBuf, size, 1, fp);
    fclose(fp);

    sends += write(imageBuf, size);

    qDebug() << "total send:" << sends;
    
}

void MyTCPSocket::sendQImage(const QByteArray& image, const QString& jsonMsgOfImage)
{
    PackageHead head;

    long size = image.size();

    memcpy(head.flagBit, "&q*b", 4);
    if (jsonMsgOfImage.length() > 0)
    {
        head.length =  jsonMsgOfImage.length();
    }
    head.cmdId = 0x0012;

    std::string strHead((char*)&head, sizeof(PackageHead));
    int sends = write(strHead.c_str(), PackageLength);

    if (jsonMsgOfImage.length() > 0)
    {
        sends += write(jsonMsgOfImage.toStdString().c_str(), jsonMsgOfImage.length());
    }

    sends += write(image.data(), size);

}

void MyTCPSocket::sendJsonWithImage(const QString& strBase64Image, const QString& jsonStr)
{
}

void MyTCPSocket::connectHost()
{
    connectToHost(kHostIp, kHostPort);
    //如果连接不上，就发出信号告诉客户端
    if (!waitForConnected(7000))
    {
        emit signalConnectResult(false);
        return;
    }
    emit signalConnectResult(true);
}

void MyTCPSocket::handleMsg()
{
}

void MyTCPSocket::initConnect()
{
}


