#include "UserChatDatabase.h"

using namespace module;

namespace database
{
    UserChatDatabase::UserChatDatabase(const char* friendId,const char* friendName):
        m_strFriendId(friendId),
        m_strFriendName(friendName)
    {
        m_id = PublicDataManager::get_mutable_instance().getMyId();
    }

    bool UserChatDatabase::createChatTable() const
    {
        QSqlQuery query;
        //每个用户都有一个自己的库，在自己的库中和每个人的聊天记录是一个表
        QString str = "CREATE TABLE chatrecord" + m_strFriendId + "(pos INTEGER PRIMARY KEY AUTOINCREMENT, message VARCHAR(50), time VARCHAR(20), isself  BOOL, name VARCHAR(30));";
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query));
        {
            return false;
        }
        return true;
    }

    bool UserChatDatabase::insertChatRecoed(int TotalCount, const QString& message, const QString& time, bool isSelf, const QString& name) const
    {
        auto strIsSelf = isSelf ? "true" : "false";
        QSqlQuery query;
        const QString str = "insert into chatrecord" + m_strFriendId + " values (" + QString::number(TotalCount + 1) + ",\"" + message + "\",\"" + time + "\"," + strIsSelf + ",\"" + name + "\")";
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "insert chat record failed");
            return false;
        }
        return true;
    }

    QString UserChatDatabase::queryFinalChatRecord() const
    {
        QSqlQuery query;
        const QString str = "select message from chatrecord" + m_strFriendId + " order by pos desc limit 1;";
        DataBaseOperate::get_mutable_instance().executeSql(str, query);
        while (query.next())
        {
            auto record = query.record();
            QString res = "";
            res = record.value("message").toString();
            return res;
        }
        return "";
    }

    bool UserChatDatabase::insertChatRecord(int TotalCount, const QString& message, const QString& time, bool isSelf, const QString& name) const
    {
        QSqlQuery query;
        auto strIsSelf = isSelf ? "true" : "false";
        const QString tmp = "insert into chatrecord" + m_strFriendId + " values (" + QString::number(TotalCount + 1) + ",\"" + message + "\",\"" + time + "\"," + strIsSelf + ",\"" + name + "\")";
        if (!DataBaseOperate::get_mutable_instance().executeSql(tmp, query))
        {
            _LOG(Logcxx::Level::ERRORS, "insert chat record failed");
            return false;
        }
        return true;
    }

    bool UserChatDatabase::queryCertainCountChatRecordAcodId(QString id, std::vector<MyChatMessageInfo>& chatMessage, int queryCount, int beginPos) const
    {
        const QString str = "select * from chatrecord" + id + " order by pos desc limit " + QString::number(beginPos) + ", " + QString::number(queryCount);
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str,query))
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
    int UserChatDatabase::queryChatRecordCountFromDB(const QString& id) const
    {
        const QString str = "select count(*) from chatrecord" + id;
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
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
}

