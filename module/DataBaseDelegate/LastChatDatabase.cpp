#include "LastChatDatabase.h"

using namespace module;

namespace database
{
    LastChatDatabase::LastChatDatabase()
    {
        m_strId = PublicDataManager::get_const_instance().getMyId();
    }
    bool LastChatDatabase::createTable() const
    {
        const QString str = "create table lastChatList (id int,isGroupChat int)";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str,query))
        {
            _LOG(Logcxx::Level::ERRORS, "create last chat list table failed");
            return false;
        }
        return true;
    }
    bool LastChatDatabase::insertLastChat(const QString& id, bool isGroupChat) const
    {
        QSqlQuery query;
        QString str = "";
        if (isGroupChat)
        {
            str = "insert into lastChatList values(" + id + ",1)";
        }
        else
        {
            str = "insert into lastChatList values(" + id + ",0)";
        }
        if (!DataBaseOperate::get_mutable_instance().executeSql(str,query))
        {
            _LOG(Logcxx::Level::ERRORS, "insert last chat failed");
            return false;
        }
        return true;
    }
    bool LastChatDatabase::insertLastChat(const std::vector<std::pair<QString, bool>>& order) const
    {
        QSqlQuery query;
        QString str = "";
        for (auto item : order)
        {
            str = "";
            if (item.second)
            {
                str = "insert into lastChatList values(" + item.first + ",1)";
            }
            else
            {
                str = "insert into lastChatList values(" + item.first + ",0)";
            }

            if (!DataBaseOperate::get_mutable_instance().executeSql(str,query))
            {
                _LOG(Logcxx::Level::ERRORS, "insert last chat failed");
                return false;
            }
        }
        return true;
    }
    bool LastChatDatabase::clearLastChat() const
    {
        const QString str = "delete from lastChatList where 1=1";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "clear last chat failed");
            return false;
        }
        return true;
    }
    bool LastChatDatabase::queryLastChatListFromDB(std::vector<MyLastChatFriendInfo>& m_tmpVec) const
    {
        if (!DataBaseOperate::get_mutable_instance().isTableExist("lastChatList"))
        {
            if (!createTable())
            {
                _LOG(Logcxx::Level::ERRORS, "create chatlastlist failed");
                return false;
            }
        }
        const QString str = "select * from lastChatList";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "query last chat list failed");
        }
        QSqlRecord record = query.record();
        while (query.next())
        {
            auto tmp = MyLastChatFriendInfo();
            record = query.record();
            tmp.m_strId = record.value("id").toString();
            tmp.m_bIsGroup = record.value("isGroupChat").toBool();
            m_tmpVec.push_back(tmp);
        }
        return false;
    }
    bool LastChatDatabase::deleteInfo() const
    {
        const QString str = "delete from lastchatlist where 1=1";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "delete last chat info failed");
            return false;
        }
        return true;
    }
}
