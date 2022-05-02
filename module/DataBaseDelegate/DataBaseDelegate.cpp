#include "DataBaseDelegate.h"
#include <QApplication>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QFile>
#include <QDir>
#include <QDebug>

using SingletonPtr = std::shared_ptr<DataBaseDelegate>;
std::mutex DataBaseDelegate::m_mutex;
SingletonPtr DataBaseDelegate::m_SingletonPtr = nullptr;

void DataBaseDelegate::init()
{
    //这里使用的是sqlite
    m_dataBase = QSqlDatabase::addDatabase("QSQLITE");
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
        //TODO 日志系统
        QMessageBox::warning(0, QObject::tr("Database Error"),
            m_dataBase.lastError().text());
        printf("open database failed\n");
        return;
    }
    if (!isTableExist("lastChatList"))
    {
        if (!createLastChatListTable())
        {
            printf("create chatlastlist failed\n");
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

void DataBaseDelegate::SetUserId(QString id)
{
    m_strUserId = id;
    init();
}

int DataBaseDelegate::GetChatRecordCountFromDB(QString id)
{
    QString str = "select count(*) from chatrecord" + id;
    QSqlQuery query;
    if (!query.exec(str))
    {
        printf("query chatrecord failed failed\n");
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

bool DataBaseDelegate::createUserChatTable(const QString& userid)
{
    //每个用户都有一个自己的库，在自己的库中和每个人的聊天记录是一个表
    QString str = "create table chatrecord" + userid + " (pos INTEGER PRIMARY KEY,message varchar(100) not null,time varchar(100) not null,isself bool,name varchar(30)";
    QSqlQuery query;
    if (!query.exec(str))
    {
        //TODO 修改为日志
        printf("create table chat record failed\n");
        return false;
    }
    return true;
}

bool DataBaseDelegate::createLastChatListTable()
{
    QString str = "create table lastChatList (id int,pos int)";
    QSqlQuery query;
    if (!query.exec(str))
    {
        //TODO 修改为日志
        printf("create table lastchatlist failed\n");
        return false;
    }
    return true;
}

bool DataBaseDelegate::createFriendRequestTable()
{
    QString str = "create table friendRequest (id int,name varchar(40),isvalid bool)";
    QSqlQuery query;
    if (!query.exec(str))
    {
        //TODO 修改为日志
        printf("create table friendlist failed\n");
        return false;
    }
    return true;
}

bool DataBaseDelegate::insertChatRecoed(int TotalCount,const QString& userid, const QString& message, const QString& time,bool isSelf, const QString& name)
{
    QSqlQuery query;
    auto strIsSelf = isSelf ? "true" : "false";
    QString tmp = "insert into chatrecord" + userid + " values ("+QString::number(TotalCount+1)+",\"" + message + "\",\"" + time + "\","+strIsSelf+",\""+name+"\")";
    if (!query.exec(tmp))
    {
        printf("insert chatrecord failed\n");
        return false;
    }
    return true;
}

bool DataBaseDelegate::isTableExist(const QString& tableNmae)
{
    if (!m_dataBase.tables().contains(tableNmae))
    {
        printf("not contains\n");
        return false;
    }
    printf("%s contains\n",tableNmae.toStdString().c_str());
    return true;
}

bool DataBaseDelegate::queryLastChatListFromDB(std::vector<QString>& m_tmpVec)
{
    QString str = "select * from lastChatList order by pos";
    QSqlQuery query;
    if (!query.exec(str))
    {
        printf("query lastchat list failed\n");
    }
    QSqlRecord record = query.record();
    while (query.next())
    {
        record = query.record();
        QString id = record.value("id").toString();
        m_tmpVec.push_back(id);
    }
    return false;
}

bool DataBaseDelegate::queryChatRecordAcodIdFromDB(QString id, std::vector<MyChatMessageInfo>& chatMessage, int queryCount, int beginPos)
{
    QString str = "select * from chatrecord" + id + " order by pos desc limit "+QString::number(beginPos)+", " + QString::number(beginPos+queryCount);
    QSqlQuery query;
    if (!query.exec(str))
    {
        printf("query chatrecord failed\n");
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

bool DataBaseDelegate::QueryInitialAcordIdFromDB(QString id, QString& str)
{
    QString strs = "select name from chatrecord" + id + " where isself=false limit 1,1";
    QSqlQuery query;
    if (!query.exec(strs))
    {
        printf("query initial failed\n");
        return false;
    }
    while (query.next())
    {
        QSqlRecord record = query.record();
        str = record.value(0).toString();
    }
    return true;
}

bool DataBaseDelegate::queryAddFriendInfoFromDB(QString id, std::vector<MyAddFriendInfo>& addFriendInfo)
{
    return true;
}

bool DataBaseDelegate::queryFriendRequestAcordId(QString id)
{
    return true;
}

