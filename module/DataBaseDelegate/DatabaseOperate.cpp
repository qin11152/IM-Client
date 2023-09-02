#include "DatabaseOperate.h"

namespace database
{

    DataBaseOperate::~DataBaseOperate()
    {
        closeDB();
    }
    DataBaseOperate::DataBaseOperate()
    {
        init();
    }

    void DataBaseOperate::closeDB()
    {
        if (m_bConnectedState)
        {
            m_dataBase.close();
            m_bConnectedState = false;
        }
        m_bConnectedState = false;
    }
    void DataBaseOperate::init()
    {
        m_dataBase = QSqlDatabase::addDatabase("QSQLITE", "sqlite1");
        //没有数据库文件夹就建立一个文件夹
        QString fileName = QApplication::applicationDirPath() + "/data";
        QDir dir(fileName);
        if (!dir.exists())
        {
            dir.mkdir(fileName);
        }
        //建立一个库，没有就建立
        QString dataName = QApplication::applicationDirPath() + "/data/chatinfo" + PublicDataManager::get_mutable_instance().getMyId() + ".db";
        m_dataBase.setDatabaseName(dataName);
        if (!m_dataBase.open())
        {
            _LOG(Logcxx::Level::ERRORS, "open data base failed");
            return;
        }
    }
    bool DataBaseOperate::executeSql(const QString& cmd, QSqlRecord& record)
    {
        record.clear();
        QSqlQuery query(m_dataBase);
        if (!query.exec(cmd))
        {
            _LOG(Logcxx::Level::ERRORS, "get chat record count failed");
            return false;
        }
        int iMessageCount = { 0 };
        record = query.record();
        return true;
    }
}