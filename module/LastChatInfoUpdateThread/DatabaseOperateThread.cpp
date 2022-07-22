#include "DatabaseOperateThread.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"
#include "module/Log/Log.h"
#include <QDir>
#include <QApplication>

DatabaseOperateThread::DatabaseOperateThread(QObject *parent)
    : QThread(parent)
{
    //��ʼ����ʱ���Ȱ����ݿ����Ӵ�
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
    //����ʹ�õ���sqlite
    m_dataBase = QSqlDatabase::addDatabase("QSQLITE");
    //û�����ݿ��ļ��оͽ���һ���ļ���
    const QString fileName = QApplication::applicationDirPath() + "/data";
    const QDir dir(fileName);
    if (!dir.exists())
    {
        if(!dir.mkdir(fileName))
        {
           _LOG(Logcxx::ERROR,"�������ݿ��ļ���ʧ��");
        }
    }
    //����һ���⣬û�оͽ���
    const QString dataName = QApplication::applicationDirPath() + "/data/thread" + ".db";
    m_dataBase.setDatabaseName(dataName);
    if (!m_dataBase.open())
    {
        //TODO ��־ϵͳ
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
            //Ȼ���ȡ��ǰ��˳��
            QStringList newModelOrder;
            ChatWidgetManager::Instance()->onSignalGetModelOrder(newModelOrder);
            //�ѵ�˳��������
            for (int i = 0; i < newModelOrder.size(); ++i)
            {
                //TODO ����һ�����뵽threadDB��lastchat�ӿ�
            }
        }
        break;
    default:
        {}
        break;
    }
}
