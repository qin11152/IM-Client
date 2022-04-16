#pragma once

#include "../MyDefine.h"
#include <QObject>
#include <QSqlDatabase>
#include <mutex>
#include <memory>
#include <vector>

class SQLDelegate : public QObject
{
    Q_OBJECT

public:
    ~SQLDelegate();
    using SingletonPtr = std::shared_ptr<SQLDelegate>;
    //����ģʽ��Ψһ��ȡ��ָ��ķ�����ʹ��������ָ�룬�Զ��ڴ����
    static SingletonPtr Instance();
    SQLDelegate(const SQLDelegate& l) = delete;
    SQLDelegate& operator=(const SQLDelegate& l) = delete;
    void init();
    std::vector<MyFriendInfo*> queryLastChatList();
    
private:
    SQLDelegate* m_ptrTcpSocket{ nullptr };      //socketָ��
    static std::mutex m_mutex;                  //������֤�̰߳�ȫ
    static SingletonPtr m_SingletonPtr;         //���������ָ��

    //���캯����˽��
    SQLDelegate(QObject* parent=nullptr);
};
