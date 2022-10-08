#pragma once

#include <QThread>
#include <QTimer>
#include "../module/MyDefine.h"
#include "module/TCPThread/MyTCPSocket.h"
#include "boost/noncopyable.hpp"
#include <boost/serialization/singleton.hpp>

struct LengthInfo
{
    int length;
    LengthInfo(int l) :length(l) {}
};

class ChatWidget;

class TCPThread  : public QThread, public boost::noncopyable, public boost::serialization::singleton<TCPThread>
{
    Q_OBJECT

public:
    TCPThread(QObject *parent=nullptr);
    ~TCPThread();

    //����ʼ��
    void unInit();
    //��дrun����
    void run()override;

public slots:
    //���øú����������������Ϣ
    void sendMessage(std::string message);
    //���øú�������ͼƬ��Ϣ
    void sendImageMsg(QString& strBase64Image, const QString& imageName);

signals:
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
    //����һ��ʱ�仹û�յ���������������
    void signalTimeoutNoHeartPackage();
    //������Ϣ�ź�
    void signalSendMsg(std::string msg);
    //����ͼƬ��Ϣ�ź�
    void signalSendImageMsg(QString& strBase64Image, const QString& imageName);

private slots:
    //����ʱ�仹û�յ��ظ�
    void onSignalTimeoutNoHeartPackage();
    //��ʱ��͸�����˷�һ��������
    void onSignalTimeoutSendHeartPackage();
    //�յ���Ϣ�Ĵ���
    void onSignalRecvMessage();
    //�յ���Ϣ�Ĵ������
    void onHandleMessage(const std::string& recvMessage);

private:
    MyTCPSocket* m_ptrTcpSocket{ nullptr };      //socketָ��
    int m_iPort{ 9999 };                        //�������˿�
    QString m_strIp{ "43.142.158.231" };         //������ip
    bool m_bIsConnected{ false };                   //�Ƿ��һ�η�����Ϣ
    QTimer* m_ptrTimerKeepAlive{ nullptr };
    QTimer* m_ptrTimerRecvHeartPackage{ nullptr };
    char m_msgBuffer[kMsgBufferLength]{ 0 };                      //���ڴ洢��Ϣ�Ļ�����
    int m_endPosOfBuffer = 0;                                     //��Ϣ��������β��

    //���������������ͼƬ��Ϣ
    std::unordered_map<std::string, std::string> m_mapImageUUIDAndBase64;
    std::unordered_map<std::string, int> m_mapImageUUIDAndSegment;

    //��������������ӵĺ���
    void connectHost();
    //��ʼ���ۺ�������
    void initConnect();
    //��ʼ������
    void init();
    //��������Ͽ�����
    void disConnect();
};
