#pragma once

#include <QObject>
#include <QTcpSocket>
#include <mutex>
#include <memory>
#include <QTimer>
#include "../module/MyDefine.h"

namespace
{
    constexpr int kHeartTimeoutTime = 300 * 1000;
    constexpr int kKeepAlivePackageTime = 299 * 1000;
    constexpr int kMsgBufferLength = 1024 * 10;
}

struct LengthInfo
{
    int length;
    LengthInfo(int l) :length(l) {}
};

class TCPConnect : public QObject
{
    Q_OBJECT

public:
    using SingletonPtr = std::shared_ptr<TCPConnect>;
    //����ģʽ��Ψһ��ȡ��ָ��ķ�����ʹ��������ָ�룬�Զ��ڴ����
    static SingletonPtr Instance();
    //���øú����������������Ϣ
    void sendMessage(std::string& message);
    void sendLength(LengthInfo& l, int length);
    ~TCPConnect();
    //���追������͸�ֵ���캯��
    TCPConnect(const TCPConnect&) = delete;
    TCPConnect& operator=(const TCPConnect&) = delete;
signals:
    //�յ�ע��������Ϣ
    void signalRecvRegisterMessage(const QString& msg);
    //�յ���¼�����Ϣ
    void signalRecvLoginResultMessage(const QString& msg);
    //�յ�����������Ϣ
    void signalRecvSingleChatMessage(const QString& msg);
    //�����������ʧ���ź�
    void signalConnectFailed();
    //����һ��ʱ�仹û�յ���������������
    void signalTimeoutNoHeartPackage();

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
    QTcpSocket* m_ptrTcpSocket{ nullptr };      //socketָ��
    static std::mutex m_mutex;                  //������֤�̰߳�ȫ
    static SingletonPtr m_SingletonPtr;         //���������ָ��
    int m_iPort{ 9999 };                        //�������˿�
    QString m_strIp{ "39.104.207.91" };         //������ip
    bool m_bIsConnected{ false };                   //�Ƿ��һ�η�����Ϣ
    QTimer* m_ptrTimerKeepAlive{ nullptr };
    QTimer* m_ptrTimerRecvHeartPackage{ nullptr };
    char m_msgBuffer[kMsgBufferLength]{ 0 };                      //���ڴ洢��Ϣ�Ļ�����
    int m_endPosOfBuffer = 0;                                     //��Ϣ��������β��

    //��������������ӵĺ���
    void connectHost();
    //��ʼ���ۺ�������
    void initConnect();
    //��ʼ������
    void init();
    //���캯����˽��
    TCPConnect(QObject* parent=nullptr);
};


