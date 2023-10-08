#include "DatabaseOperate.h"

namespace database
{

    using CreateTableAccordingName = std::function<bool(const QString&)>;

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
        initTables();
        return true;
    }
    void DataBaseOperate::initTables()
    {
        std::map<std::string, std::string> kTableNameAndCmd;
        kTableNameAndCmd["lastChatList"] = "create table lastChatList (id int,isGroupChat int)";
        kTableNameAndCmd["profileImage"] = "create table profileImage (id varchar(10),imagePath varchar(100),timestamp varchar(30))";
        kTableNameAndCmd["friendRequest"] = "create table friendRequest (id int,name varchar(40),isvalid bool,createdtime time,verifymessage varchar(30))";
        CreateTableAccordingName creatFunc = std::bind(&DataBaseOperate::executeSqlWithoutReturn, this, std::placeholders::_1);
        for (auto& item : kTableNameAndCmd)
        {
            if (!isTableExist(item.first.c_str()))
            {
                creatFunc(item.second.c_str());
            }
        }
    }
    bool DataBaseOperate::executeSql(const QString& cmd, QSqlQuery& query)
    {
        assert(m_threadId == std::this_thread::get_id());
        query= QSqlQuery(m_dataBase);
        if (!query.exec(cmd))
        {
            auto str = query.lastError().databaseText();
            _LOG(Logcxx::Level::ERRORS, "exec sql failed,error is:", str.toStdString().c_str());
            return false;
        }
        return true;
    }

    bool DataBaseOperate::executeSqlWithoutReturn(const QString& cmd)
    {
        assert(m_threadId == std::this_thread::get_id());
        QSqlQuery query(m_dataBase);
        if (!query.exec(cmd))
        {
            auto str = query.lastError().databaseText();
            _LOG(Logcxx::Level::ERRORS, "exec sql failed,error is:", str.toStdString().c_str());
            return false;
        }
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

    using SingletonPtr = std::shared_ptr<DatabaseOperateForQml>;
    //初始化静态成员函数
    SingletonPtr DatabaseOperateForQml::m_SingletonPtr = nullptr;

    std::mutex DatabaseOperateForQml::m_mutex;

    SingletonPtr DatabaseOperateForQml::instance()
    {
        //双重保护，第一重判空，避免生成多个
        if (m_SingletonPtr == nullptr)
        {
            std::lock_guard<std::mutex> lck(m_mutex);
            //加锁后判空，避免多线程生成多个实例
            if (m_SingletonPtr == nullptr)
            {
                m_SingletonPtr = std::shared_ptr<DatabaseOperateForQml>(new DatabaseOperateForQml);
            }
        }
        //返回指针
        return m_SingletonPtr;
    }

    int DatabaseOperateForQml::getChatRecordCountFromDB(const QString& strId)
    {
        const QString str = "select count(*) from chatrecord" + strId;
        QSqlQuery query;
        if (!database::DataBaseOperate::get_mutable_instance().executeSql(strId, query))
        {
            _LOG(Logcxx::Level::ERRORS, "get chat record count failed");
        }
        int iMessageCount = { 0 };
        QSqlRecord record = query.record();
        while (query.next())
        {
            record = query.record();
            iMessageCount = record.value(0).toInt();
            break;
        }
        return iMessageCount;
    }
    DatabaseOperateForQml::DatabaseOperateForQml(QObject* parent)
        :QObject(parent)
    {
    }
}