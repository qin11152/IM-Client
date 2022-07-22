#include "DatabaseOperateThread.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"
#include "module/Log/Log.h"
#include <QDir>
#include <QApplication>

DatabaseOperateThread::DatabaseOperateThread(QObject *parent)
    : QThread(parent)
{
    //初始化的时候，先把数据库连接打开
}

void DatabaseOperateThread::setOperateType(const DatabaseOperateType& operateType)
{
    m_threadOperateType = operateType;;
}

void DatabaseOperateThread::setCurUserId(const QString& curId)
{
    m_strCurrentUserId = curId;
}

void DatabaseOperateThread::init()
{
    //这里使用的是sqlite
    m_dataBase = QSqlDatabase::addDatabase("QSQLITE");
    //没有数据库文件夹就建立一个文件夹
    const QString fileName = QApplication::applicationDirPath() + "/data";
    const QDir dir(fileName);
    if (!dir.exists())
    {
        if(!dir.mkdir(fileName))
        {
           _LOG(Logcxx::ERROR,"创建数据库文件夹失败");
        }
    }
    //建立一个库，没有就建立
    const QString dataName = QApplication::applicationDirPath() + "/data/thread" + ".db";
    m_dataBase.setDatabaseName(dataName);
    if (!m_dataBase.open())
    {
        //TODO 日志系统
        _LOG(Logcxx::ERROR, "open data base failed");
    }
}

bool DatabaseOperateThread::isLastChatExist()
{
}

void DatabaseOperateThread::insertIntoLastChat()
{
}

void DatabaseOperateThread::run()
{

    switch (static_cast<int>(m_threadOperateType))
    {
    case static_cast<int>(DatabaseOperateType::None):
        {}
        break;
        case static_cast<int>(DatabaseOperateType::UpdateLastChat):
        {
            //然后获取当前的顺序
            QStringList newModelOrder;
            ChatWidgetManager::Instance()->onSignalGetModelOrder(newModelOrder);
            //把的顺序存入表中
            for (int i = 0; i < newModelOrder.size(); ++i)
            {
                //TODO 创建一个插入到threadDB中lastchat接口
            }
        }
        break;
    default:
        {}
        break;
    }
}
