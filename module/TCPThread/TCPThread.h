#pragma once


#include "module/stdafx.h"
#include "module/TCPThread/MyTCPSocket.h"

#include "boost/noncopyable.hpp"
#include <boost/serialization/singleton.hpp>

#include <QThread>
#include <QTimer>

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
    void sendImageMsg(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);
    //���ʹ���ͼƬ����Ϣ(base64)
    void sendJsonWithImage(const QString& strBase64Image, const QString& jsonStr);
    //����ͼƬ������ͷ��+json�ַ���+ͼƬ��Ϣ
    /**
     * brief������ͼƬ��Ϣ�Ľӿڣ���ͼƬ��Ϣ����������json��.
     * 
     * \param filePath��Ҫ���͵�ͼƬ��·��
     * \param jsonMsgOfImage������ͼƬ��Ϣ��json�ַ���
     */
    void sendImage(const char* filePath, const char* jsonMsgOfImage = "");

    void sendImage(const QByteArray& image, const char* jsonMsgOfImage = "");



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
    //ĳ������ͷ������˵��ź�
    void signalProfileImageChanged(const QString& id,const QString& path);
    //��Ӻ���ʱ���������͹�����ͷ����Ϣ
    void signalAddFriendProfileImage(const QString& id, const QString& path);

    /////Ϊ���ܹ������߳��з��ͽ�����Ϣ����Ҫ�ٷ�װһ��
    //������Ϣ�ź�
    void signalSendMsg(const std::string& msg);
    //����ͼƬ��Ϣ�ź�
    void signalSendImageMsg(const QString& strBase64Image, const QString& imageName, const QString& suffix, const QString& timeStamp);
    //����ͼƬ��json��Ϣ�ź�
    void signalSendJsonWithImage(const QString& strBase64Image, const QString& jsonStr);

    //���ӵ��ź�
    void signalConnect();
    //��������ͼƬ���ź�
    void signalSendIma(const QString& filePath,const QString& jsonMsg);
    void signalQtSendIma(const QByteArray& image, const QString& jsonMsg);

private slots:
    //����ʱ�仹û�յ��ظ�
    void onSignalTimeoutNoHeartPackage();
    //��ʱ��͸�����˷�һ��������
    void onSignalTimeoutSendHeartPackage();
    //�յ���Ϣ�Ĵ���
    void onSignalRecvMessage();
    //�յ���Ϣ�Ĵ������
    void onHandleMessage(const std::string& recvMessage);
    //�յ����ӵĽ��
    void onConnectResult(bool bResult);

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

