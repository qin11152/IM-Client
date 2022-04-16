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
    //单例模式，唯一获取其指针的方法，使用了智能指针，自动内存管理
    static SingletonPtr Instance();
    SQLDelegate(const SQLDelegate& l) = delete;
    SQLDelegate& operator=(const SQLDelegate& l) = delete;
    void init();
    std::vector<MyFriendInfo*> queryLastChatList();
    
private:
    SQLDelegate* m_ptrTcpSocket{ nullptr };      //socket指针
    static std::mutex m_mutex;                  //锁，保证线程安全
    static SingletonPtr m_SingletonPtr;         //该类的智能指针

    //构造函数，私有
    SQLDelegate(QObject* parent=nullptr);
};
