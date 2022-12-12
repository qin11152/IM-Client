#include "DataBaseDelegate.h"
#include "module/Log/Log.h"
#include <QApplication>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>
#include <QDebug>

using SingletonPtr = std::shared_ptr<DataBaseDelegate>;
std::mutex DataBaseDelegate::m_mutex;
SingletonPtr DataBaseDelegate::m_SingletonPtr = nullptr;

using CreateTableWithoutParams = std::function<bool()>;
std::map<std::string, CreateTableWithoutParams> kTableAndFuncWithoutParam;

void DataBaseDelegate::init()
{
    kTableAndFuncWithoutParam["lastChatList"] = std::bind(&DataBaseDelegate::createLastChatListTable,this);
    kTableAndFuncWithoutParam["profileImage"] = std::bind(&DataBaseDelegate::createProfileImageTable, this);
    kTableAndFuncWithoutParam["friendRequest"] = std::bind(&DataBaseDelegate::createFriendRequestTable, this);
    //这里使用的是sqlite
    m_dataBase = QSqlDatabase::addDatabase("QSQLITE","sqlite1");
    //没有数据库文件夹就建立一个文件夹
    QString fileName = QApplication::applicationDirPath() + "/data";
    QDir dir(fileName);
    if(!dir.exists())
    {
        dir.mkdir(fileName);
    }
    //建立一个库，没有就建立
    QString dataName = QApplication::applicationDirPath() + "/data/chatinfo" + m_strUserId + ".db";
    m_dataBase.setDatabaseName(dataName);
    if (!m_dataBase.open())
    {
        QMessageBox::warning(0, QObject::tr("Database Error"),
            m_dataBase.lastError().text());
        _LOG(Logcxx::Level::ERRORS, "open data base failed");
        return;
    }
    initTables();
}

void DataBaseDelegate::disConnect()
{
    m_dataBase.close();
    QString fileName = QApplication::applicationDirPath() + "/data";
    QString dataName = QApplication::applicationDirPath() + "/data/chatinfo" + m_strUserId + ".db";
    QSqlDatabase::removeDatabase(dataName);
}

void DataBaseDelegate::initTables()
{
    for (auto& item : kTableAndFuncWithoutParam)
    {
        if (!isTableExist(item.first.c_str()))
        {
            item.second();
        }
    }
}

DataBaseDelegate::DataBaseDelegate(QObject *parent)
    : QObject(parent)
{
}

SingletonPtr DataBaseDelegate::Instance()
{
    if (m_SingletonPtr == nullptr)
    {
        std::lock_guard<std::mutex> lck(m_mutex);
        if (m_SingletonPtr == nullptr)
        {
            m_SingletonPtr = std::shared_ptr<DataBaseDelegate>(new DataBaseDelegate);
        }
    }
    return m_SingletonPtr;
}

DataBaseDelegate::~DataBaseDelegate()
{
    m_dataBase.close();
}

void DataBaseDelegate::setUserId(const QString& id)
{
    m_strUserId = id;
}

int DataBaseDelegate::getChatRecordCountFromDB(const QString& id)const
{
    const QString str = "select count(*) from chatrecord" + id;
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "get chat record count failed");
    }
    int iMessageCount={ 0 };
    QSqlRecord record = query.record();
    while (query.next())
    {
        record = query.record();
        iMessageCount = record.value(0).toInt();
        break;
    }
    return iMessageCount;
}

bool DataBaseDelegate::createUserChatTable(const QString& userid)const
{
    //每个用户都有一个自己的库，在自己的库中和每个人的聊天记录是一个表
    //QString str = "create table chatrecord" + userid + " (pos INTEGER PRIMARY KEY,message varchar(100) not null,time varchar(100) not null,isself bool,name varchar(30)";
    const QString str = "CREATE TABLE chatrecord" + userid + "(pos INTEGER PRIMARY KEY AUTOINCREMENT, message VARCHAR(50), time VARCHAR(20), isself  BOOL, name VARCHAR(30));";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "create chat record table failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::createLastChatListTable()const
{
    const QString str = "create table lastChatList (id int)";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "create last chat list table failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::createProfileImageTable() const
{
    const QString str = "create table profileImage (id varchar(10),imagePath varchar(100),timestamp varchar(30))";
    QSqlQuery query(m_dataBase);
    if(!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "create profile image table failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::createFriendRequestTable()const
{
    const QString str = "CREATE TABLE friendRequest (id INT,name VARCHAR(40),isvalid BOOL,createdtime TIME,verifymessage VARCHAR(30))";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "create friend request table failed");
        return false;
    }
    return true;
}

QString DataBaseDelegate::queryLastChatRecord(const QString& id) const
{
    QString strRecord = "";
    const QString str = "select message from chatrecord" + id + " order by pos desc limit 1;";
    QSqlQuery query(m_dataBase);
    if(!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "query last chat record failed");
    }
    QSqlRecord record = query.record();
    while (query.next())
    {
        record = query.record();
        strRecord = record.value("message").toString();
    }
    return strRecord;
}

//插入是插在最后边的，也就是最后一个插入的在表的最前边
//而我们默认表中的顺序就是显示的顺序，所以要显示的第一个要最后一个插
bool DataBaseDelegate::insertLastChat(const QString& id)const
{
    QSqlQuery query(m_dataBase);
    const QString str= "insert into lastChatList values(" + id + ")";
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "insert last chat failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::insertLastChat(const std::vector<QString>& order) const
{
    QSqlQuery query(m_dataBase);
    QString str = "";
    for(auto id:order)
    {
        str = "insert into lastChatList values(" + id + ");";
        if (!query.exec(str))
        {
            _LOG(Logcxx::Level::ERRORS, "insert last chat failed");
            return false;
        }
    }
    return true;
}

bool DataBaseDelegate::clearLastChat() const
{
    const QString str="delete from lastChatList where 1=1";
    QSqlQuery query(m_dataBase);
    if(!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "clear last chat failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::insertChatRecoed(int TotalCount,const QString& userid, const QString& message, const QString& time,bool isSelf, const QString& name)const
{
    QSqlQuery query(m_dataBase);
    auto strIsSelf = isSelf ? "true" : "false";
    const QString tmp = "insert into chatrecord" + userid + " values ("+QString::number(TotalCount+1)+",\"" + message + "\",\"" + time + "\","+strIsSelf+",\""+name+"\")";
    if (!query.exec(tmp))
    {
        _LOG(Logcxx::Level::ERRORS, "insert chat record failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::insertAddFriendRequest(const QString& id, const QString& name, const QString& verifyMsg)const
{
    if (!DataBaseDelegate::Instance()->isTableExist("friendRequest"))
    {
        DataBaseDelegate::Instance()->createFriendRequestTable();
    }
    const QString str = "insert into friendRequest values(" + id + ",'" + name + "',false,datetime('now','localtime'),'" + verifyMsg + "')";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "insert add friend request failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::insertProfilePathAndTimestamp(const QString& id, const QString& path, const QString& timestamp)const
{
    const QString str = "insert into profileImage values(" + id + ",'" + path + "','" + timestamp + "')";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "insert into profile image failed,sql is:%s", str.toStdString().c_str());
        return false;
    }
    return true;
}

bool DataBaseDelegate::isTableExist(const QString& tableName)const
{
    if (!m_dataBase.tables().contains(tableName))
    {
        _LOG(Logcxx::Level::ERRORS, "table not exist");
        return false;
    }
    return true;
}

bool DataBaseDelegate::queryLastChatListFromDB(std::vector<MyLastChatFriendInfo>& m_tmpVec)const
{
    if (!isTableExist("lastChatList"))
    {
        if (!createLastChatListTable())
        {
            _LOG(Logcxx::Level::ERRORS, "create chatlastlist failed");
        }
    }
    const QString str = "select * from lastChatList";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "query last chat list failed");
    }
    QSqlRecord record = query.record();
    while (query.next())
    {
        auto tmp = MyLastChatFriendInfo();
        record = query.record();
        tmp.m_strId = record.value("id").toString();
        m_tmpVec.push_back(tmp);
    }
    return false;
}

bool DataBaseDelegate::queryChatRecordAcodIdFromDB(QString id, std::vector<MyChatMessageInfo>& chatMessage, int queryCount, int beginPos)const
{
    const QString str = "select * from chatrecord" + id + " order by pos desc limit "+QString::number(beginPos)+", " + QString::number(queryCount);
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "query chat record failed");
        return false;
    }
    QSqlRecord record;
    while (query.next())
    {
        record = query.record();
        QString message = record.value("message").toString();
        QString name = record.value("name").toString();
        QString time = record.value("time").toString();
        bool isSelf = record.value("isself").toBool();
        chatMessage.emplace_back(MyChatMessageInfo(message, name, time, isSelf));
    }
    return true;
}

bool DataBaseDelegate::queryInitialAcordIdFromDB(const QString& id, QString& str)const
{
    const QString strs = "select name from chatrecord" + id + " where isself=false limit 1,1";
    QSqlQuery query(m_dataBase);
    if (!query.exec(strs))
    {
        _LOG(Logcxx::Level::ERRORS, "query initial acord id failed");
        return false;
    }
    while (query.next())
    {
        QSqlRecord record = query.record();
        str = record.value(0).toString();
    }
    return true;
}

bool DataBaseDelegate::queryAddFriendInfoFromDB(QString id, std::vector<MyAddFriendInfo>& addFriendInfo)const
{
    deleteExpiredFriendRequest();
    const QString str = "select * from friendRequest";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "query add friend info failed");
        return false;
    }
    while (query.next())
    {
        QSqlRecord record = query.record();
        MyAddFriendInfo tmpInfo;
        tmpInfo.m_strFriendId = record.value(0).toString();
        tmpInfo.m_strFriendName = record.value(1).toString();
        tmpInfo.m_bIsValid = record.value(2).toBool();
        tmpInfo.m_strVerifyMsg = record.value(4).toString();
        addFriendInfo.emplace_back(tmpInfo);
    }
    return true;
}

bool DataBaseDelegate::queryFriendRequestAcordName(const QString& name,QString& id)const
{
    const QString strs = "select id from friendRequest where name = '" + name+"'";
    QSqlQuery query(m_dataBase);
    if (!query.exec(strs))
    {
        _LOG(Logcxx::Level::ERRORS, "query friend request acord name failed");
        return false;
    }
    while (query.next())
    {
        QSqlRecord record = query.record();
        //查询到这个id
        id = record.value(0).toString();
    }
    return true;
}

bool DataBaseDelegate::queryProfileImagePath(const QString& id, QString& path) const
{
    const QString str = "select imagePath from profileImage where id=" + id;
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "query profile image path failed");
        return false;
    }
    while (query.next())
    {
        QSqlRecord record = query.record();
        path = record.value(0).toString();
    }
    return true;
}

bool DataBaseDelegate::queryIsIdExistInProfile(const QString& id)const
{
    const QString str = "select count(*) from profileImage where id=" + id;
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "query is id exist from profile image failed");
        return false;
    }
    int num = 0;
    QSqlRecord record;
    while (query.next())
    {
        record = query.record();
        num = record.value(0).toInt();
    }
    return num > 0;
}

bool DataBaseDelegate::queryProfileTimeStamp(std::unordered_map<std::string, std::string>& mapTimeStamp) const
{
    const QString str = "select id, timestamp from profileImage";
    QSqlQuery query(m_dataBase);
    if(!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "select friend image timestamp failed");
        return false;
    }

    while (query.next())
    {
        QSqlRecord record = query.record();
        //查询到这个id
        std::string id = record.value(0).toString().toStdString();
        const std::string timeStamp = record.value(2).toString().toStdString();
        mapTimeStamp[id] = timeStamp;
    }
    return true;
}

bool DataBaseDelegate::updateFriendRequestStateAcordId(const QString& id)const
{
    const QString str = "update friendRequest set isvalid=true where id =" + id;
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "update friend request state acord id failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::updateFriendImageTimestamp(QString& id, std::pair<QString, QString>& newInfo) const
{
    QString str = "update profileImage set timestamp='" + newInfo.second + "' where id=" + newInfo.first;
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "update friend image timestamp%d failed",id.toInt());
        return false;
    }
    return true;
}

bool DataBaseDelegate::updateProfileImagePath(const QString& id, const QString& path) const
{
    const QString str = "update profileImage set imagePath ='" + path + "' where id =" + id;
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "update profile image path failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::updateProfilleImagePathAndTimeStamp(const QString& id, const QString& path, const QString& timeStamp)const
{
    const QString str = "update profileImage set imagePath ='" + path + "',timestamp='" + timeStamp + "'where id =" + id;
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "update profile image path and time failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::deleteExpiredFriendRequest()const
{
    const QString str = "DELETE FROM friendRequest WHERE date('now', '-30 day' ,'localtime') >= date(CreatedTime)";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "delete expired friend request failed");
        return false;
    }
    return true;
}

bool DataBaseDelegate::deleteLastChatInfo()const
{
    const QString str = "delete from lastchatlist where 1=1";
    QSqlQuery query(m_dataBase);
    if (!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "delete last chat info failed");
        return false;
    }
    return true;
}

