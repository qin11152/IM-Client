#pragma once

#include "boost/noncopyable.hpp"
#include <QTCPSocket>

struct PackageHead
{
    char flagBit[4]{ 0 };
    unsigned int length;
    unsigned char cmdId;
};
constexpr int PackageLength = sizeof(PackageHead);

class MyTCPSocket : public QTcpSocket, public boost::noncopyable
{
    Q_OBJECT

public:
    MyTCPSocket(QObject* parent = nullptr);
    ~MyTCPSocket();

signals:
    void signalConnectResult(bool bResult);

public slots:
    //������Ϣ�ۺ����������װһ�㣬�������ֶ��߳����⣨QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread��
    void sendMsg(const std::string& msg);
    //����ͼƬ��Ϣ�ۺ���
    void sendImageMsg(const QString& strBase64Image, const QString& ImageName, const QString& suffix, const QString& timeStamp);
    
    //����ͼƬ
    void sendImage(const QString& filePath, const QString& jsonMsgOfImage);
    //����qimage
    void sendQImage(const QByteArray& image, const QString& jsonMsgOfImage);

    //���ʹ���ͼƬ��json��Ϣ
    void sendJsonWithImage(const QString& strBase64Image, const QString& jsonStr);
    //����������
    void connectHost();

private slots:
    //��Ϣ���Լ��߳��ڲ�����
    void handleMsg();

private:
    void initConnect();
    
};
