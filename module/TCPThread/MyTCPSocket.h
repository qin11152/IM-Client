#pragma once

#include "boost/noncopyable.hpp"
#include <QTCPSocket>

namespace module
{
    struct PackageHead
    {
        char flagBit[4]{ 0 };
        unsigned int length;
        unsigned char cmdId;
    };
    constexpr size_t PackageLength = sizeof(PackageHead);

    class MyTCPSocket : public QTcpSocket, public boost::noncopyable
    {
        Q_OBJECT

    public:
        MyTCPSocket(QObject* parent = nullptr);
        ~MyTCPSocket();

    signals:
        void signalConnectResult(bool bResult);

    public slots:
        //发送消息槽函数，必须封装一层，否则会出现多线程问题（QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread）
        void sendMsg(const std::string& msg);
        //发送图片消息槽函数
        void sendImageMsg(const QString& strBase64Image, const QString& ImageName, const QString& suffix, const QString& timeStamp);

        //发送图片
        void sendImage(const QString& filePath, const QString& jsonMsgOfImage);
        //发送qimage
        void sendQImage(const QByteArray& image, const QString& jsonMsgOfImage);

        //发送带有图片的json消息
        void sendJsonWithImage(const QString& strBase64Image, const QString& jsonStr);
        //与主机连接
        void connectHost();

    private slots:
        //消息在自己线程内部处理
        void handleMsg();

    private:
        void initConnect();

    };
}
