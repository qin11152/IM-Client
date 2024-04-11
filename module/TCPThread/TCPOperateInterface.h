#pragma once
#include "module/stdafx.h"
#include "boost/serialization/singleton.hpp"

#include <QObject>
#include <qtcpsocket.h>
#include <qtimer.h>
#include <qthread.h>

namespace module
{
    class TCPOperateInterface;
    class TCPOperate : public QObject
    {
        Q_OBJECT

    public:
        TCPOperate(TCPOperateInterface* ptr, QObject* parent);
        ~TCPOperate();

    signals:
        //���øú����������������Ϣ
        void sendMessage(std::string message);

        //����һ��ʱ�仹û�յ���������������
        void signalTimeoutNoHeartPackage();

        //�յ�ע��������Ϣ
        void signalRecvRegisterMessage(const QString& msg);
        //�յ���¼�����Ϣ
        void signalRecvLoginResultMessage(const QString& msg);
        //�յ�����������Ϣ
        void signalRecvSingleChatMessage(const QString& msg);
        //�յ������б���Ϣ
        void signalRecvFriendListMessage(const QString& msg);
        //����˷������ĺ�������
        void signalNewFriendRequest(const QString& msg);
        //����˷�������Ӻ��Ѻ��֪ͨ
        void signalBecomeFriendNotify(const QString& msg);
        //�����������ʧ���ź�
        void signalConnectFailed();
        //ĳ������ͷ������˵��ź�
        void signalProfileImageChanged(const QString& id, const QString& path);
        //��Ӻ���ʱ���������͹�����ͷ����Ϣ
        void signalAddFriendProfileImage(const QString& id, const QString& timeStamp, const QString& path);
        //Ⱥ�����
        void signalStartGroupChatReply(const QString& msg);

        /////Ϊ���ܹ������߳��з��ͽ�����Ϣ����Ҫ�ٷ�װһ��
        //������Ϣ�ź�
        void signalSendMsg(const std::string& msg);
        //����ͼƬ��Ϣ�ź�
        void signalSendImageMsg(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);

    public slots:
        /**
         * brief:tcp���ͽ����߳̿�ʼ����.
         *
         */
        void init();

        void unInit();

        void sendMessageInternal(std::string message);

        void sendImageInternel(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);

        void onSignalTimeoutSendHeartPackage();

        void onSignalTimeoutNoHeartPackage();

        void onSignalRecvMessage();

        void connectHost();

        void disconnectHost();

    private:
        void onHandleMessage(const std::string& recvMessage);

    private:
        QTcpSocket* m_ptrTCPSocket{ nullptr };
        TCPOperateInterface* m_ptrExternal{ nullptr };

        bool m_bConnectResult{ false };
        int m_iPort{ 19999 };                        //�������˿�
        QString m_strIp{ "43.142.158.231" };         //������ip
        QTimer* m_timerKeepAlive{ nullptr };
        //QTimer m_timerRecvHeartPackage;
        char m_msgBuffer[kMsgBufferLength]{ 0 };                      //���ڴ洢��Ϣ�Ļ�����
        size_t m_endPosOfBuffer = 0;                                     //��Ϣ��������β��
        //���������������ͼƬ��Ϣ
        std::unordered_map<std::string, std::string> m_mapImageUUIDAndBase64;
        std::unordered_map<std::string, int> m_mapImageUUIDAndSegment;
    };

    class TCPOperateInterface : public QObject, public boost::serialization::singleton<TCPOperateInterface>
    {
        Q_OBJECT
    public:
        TCPOperateInterface();
        ~TCPOperateInterface();

        void sendMessageExternalInterface(std::string message);

        //���øú�������ͼƬ��Ϣ
        void sendImageMsgExternalInterface(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);

        void disconnect();

    signals:
        void signalSendMessageExternal(std::string message);

        //���øú�������ͼƬ��Ϣ
        void signalSendImageMsgExternal(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);


        //�յ�ע��������Ϣ
        void signalRecvRegisterMessage(const QString& msg);
        //�յ���¼�����Ϣ
        void signalRecvLoginResultMessage(const QString& msg);
        //�յ�����������Ϣ
        void signalRecvSingleChatMessage(const QString& msg);
        //�յ������б���Ϣ
        void signalRecvFriendListMessage(const QString& msg);
        //����˷������ĺ�������
        void signalNewFriendRequest(const QString& msg);
        //����˷�������Ӻ��Ѻ��֪ͨ
        void signalBecomeFriendNotify(const QString& msg);
        //�����������ʧ���ź�
        void signalConnectFailed();
        //ĳ������ͷ������˵��ź�
        void signalProfileImageChanged(const QString& id, const QString& path);
        //��Ӻ���ʱ���������͹�����ͷ����Ϣ
        void signalAddFriendProfileImage(const QString& id, const QString& timeStamp, const QString& path);
        //Ⱥ�����
        void signalStartGroupChatReply(const QString& msg);

    private:
        void initConnect();

    private:
        QThread m_operateThread;
        TCPOperate* m_ptrTCPOperate{ nullptr };
    };
}
