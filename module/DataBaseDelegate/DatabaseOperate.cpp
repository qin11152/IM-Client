#include "DatabaseOperate.h"

namespace database
{

    DataBaseOperate::~DataBaseOperate()
    {
        closeDB();
    }
    DataBaseOperate::DataBaseOperate()
    {
        m_threadId = std::this_thread::get_id();
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
    
    bool DataBaseOperate::init()
    {
        m_dataBase = QSqlDatabase::addDatabase("QSQLITE", "sqlitespecial");
        //没有数据库文件夹就建立一个文件夹
        QString fileName = QApplication::applicationDirPath() + "/data";
        QDir dir(fileName);
        if (!dir.exists())
        {
            dir.mkdir(fileName);
        }
        //建立一个库，没有就建立
        QString dataName = QApplication::applicationDirPath() + "/data/chatinfo" +PublicDataManager::get_mutable_instance().getMyId() + ".db";
        m_dataBase.setDatabaseName(dataName);
        if (!m_dataBase.open())
        {
            _LOG(Logcxx::Level::ERRORS, "open data base failed");
            return false;
        }
        return true;
    }
    bool DataBaseOperate::executeSql(const QString& cmd, QSqlQuery& query)
    {
        assert(m_threadId == std::this_thread::get_id());
        query= QSqlQuery(m_dataBase);
        if (!query.exec(cmd))
        {
            _LOG(Logcxx::Level::ERRORS, "exec sql failed");
            auto tmperror = query.lastError();
            auto str = tmperror.databaseText();
            return false;
        }
        int iMessageCount = { 0 };
        return true;
    }
    bool DataBaseOperate::isTableExist(const QString& tableName) const
    {
        if (!m_dataBase.tables().contains(tableName))
        {
            //_LOG(Logcxx::Level::ERRORS, "table not exist");
            return false;
        }
        return true;
    }
}