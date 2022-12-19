#include "module/Log/Log.h"
#include "DatabaseOperateThread.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"

#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QApplication>


DatabaseOperateThread::DatabaseOperateThread(QObject *parent)
    : QThread(parent)
{
    //初始化的时候，先把数据库连接打开
}

void DatabaseOperateThread::setOperateType(const DatabaseOperateType& operateType)
{
    m_threadOperateType = operateType;
}

void DatabaseOperateThread::setCurUserId(const QString& curId)
{
    m_strCurrentUserId = curId;
}

void DatabaseOperateThread::init()
{
    //这里使用的是sqlite
    m_dataBase = QSqlDatabase::addDatabase("QSQLITE","sqlite3");
    //没有数据库文件夹就建立一个文件夹
    const QString fileName = QApplication::applicationDirPath() + "/data";
    const QDir dir(fileName);
    if (!dir.exists())
    {
        if(!dir.mkdir(fileName))
        {
           _LOG(Logcxx::Level::ERRORS,"创建数据库文件夹失败");
        }
    }
    //建立一个库，没有就建立
    const QString dataName = QApplication::applicationDirPath() + "/data/thread" + ".db";
    m_dataBase.setDatabaseName(dataName);
    if (!m_dataBase.open())
    {
        _LOG(Logcxx::Level::ERRORS, "open data base failed");
    }
}

void DatabaseOperateThread::setLastChatList(QStringList& lastChatList)
{
    m_lastChatList = lastChatList;
}

//查询某个表是否存在
bool DatabaseOperateThread::isTableExist(const QString& tableName)const
{
    QSqlQuery query(m_dataBase);
    query.exec("select count(*) from sqlite_master where type='table' and name='" + tableName + "'");
    if (query.next())
    {
        if (query.value(0).toInt() > 0)
        {
            return true;
        }
    }
    return false;
}

//创造一个lastchat表
bool DatabaseOperateThread::createLastChat()const
{
    const QString str = "create table lastChatList" + m_strCurrentUserId + " (id int)";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "create lastChatList table failed");
        return false;
    }
    return true;
}

bool DatabaseOperateThread::clearLastChat() const
{
    const QString str = "delete from lastchatlist" + m_strCurrentUserId + " where 1=1";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "clear lastChatList table failed");
        return false;
    }
    return true;
}

bool DatabaseOperateThread::insertLastChat(const QString& id)const
{
    const QString str = "insert into lastChatList" + m_strCurrentUserId + " values(" + id + ")";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "insert lastChatList table failed");
        return false;
    }
    return true;
}

void DatabaseOperateThread::getLastChatOrder(QStringList& modelOrder) const
{
    QVariant tmp;
    QMetaObject::invokeMethod(m_ptrLastChatQML, "getModelInfo", Qt::QueuedConnection ,Q_RETURN_ARG(QVariant, tmp));
    modelOrder = tmp.toStringList();
}

void DatabaseOperateThread::run()
{
    _LOG(Logcxx::Level::INFO, "DatabaseOperateThread run");
    switch (static_cast<int>(m_threadOperateType))
    {
    case static_cast<int>(DatabaseOperateType::None):
        {}
        break;
    case static_cast<int>(DatabaseOperateType::UpdateLastChat):
        {
            //然后获取当前的顺序
            //QStringList newModelOrder;
            //getLastChatOrder(newModelOrder);
            //把的顺序存入表中
            //先看一下表是否存在，如果不存在，就创建一个表
            if(!isTableExist("lastchatlist" + m_strCurrentUserId))
            {
                createLastChat();
            }
            clearLastChat();
            for (int i = 0; i < m_lastChatList.size(); ++i)
            {
                insertLastChat(m_lastChatList.at(i));
            }
        }
        break;
    default:
        {}
        break;
    }
}
