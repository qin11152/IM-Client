#include "SQLDelegate.h"
#include <QApplication>

using SingletonPtr = std::shared_ptr<SQLDelegate>;
//初始化静态成员函数
SingletonPtr SQLDelegate::m_SingletonPtr = nullptr;
std::mutex SQLDelegate::m_mutex;

SQLDelegate::SQLDelegate(QObject *parent=nullptr)
    : QObject(parent)
{
}

void SQLDelegate::init()
{
    //在构造函数中和数据库进行连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //如果有则在本目录下生成，如果没有就会建立这个数据库
    db.setDatabaseName(QApplication::applicationDirPath() + "/Record.dat");
    if(!db.open())
    {
        //TODO日志输出，记录错误
    }
}

SQLDelegate::~SQLDelegate()
{
}

SingletonPtr SQLDelegate::Instance()
{
    //双重保护，第一重判空，避免生成多个
    if (m_SingletonPtr == nullptr)
    {
        std::lock_guard<std::mutex> lck(m_mutex);
        //加锁后判空，避免多线程生成多个实例
        if (m_SingletonPtr == nullptr)
        {
            m_SingletonPtr = std::shared_ptr<SQLDelegate>(new SQLDelegate);
        }
    }
    //返回指针
    return m_SingletonPtr;
}
