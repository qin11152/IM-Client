#include "AddFriendDatabase.h"

using namespace module;
namespace database
{
    AddFriendDatabase::AddFriendDatabase()
    {
        m_strId = PublicDataManager::get_const_instance().getMyId();
    }
    bool AddFriendDatabase::createTable() const
    {
        const QString str = "CREATE TABLE friendRequest (id INT,name VARCHAR(40),isvalid BOOL,createdtime TIME,verifymessage VARCHAR(30))";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str,query))
        {
            _LOG(Logcxx::Level::ERRORS, "create friend request table failed");
            return false;
        }
        return true;
    }
    bool AddFriendDatabase::insertAddFriendRequest(const QString& id, const QString& name, const QString& verifyMsg) const
    {
        if (!DataBaseOperate::get_const_instance().isTableExist("friendRequest"))
        {
            if (!createTable())
            {
                return false;
            }
        }
        const QString str = "insert into friendRequest values(" + id + ",'" + name + "',false,datetime('now','localtime'),'" + verifyMsg + "')";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "insert add friend request failed");
            return false;
        }
        return true;
    }
    bool AddFriendDatabase::queryAddFriendInfoFromDB(std::vector<MyAddFriendInfo>& addFriendInfo) const
    {
        deleteExpiredFriendRequest();
        const QString str = "select * from friendRequest";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
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
    bool AddFriendDatabase::updateFriendRequestStateAcordId(const QString& id) const
    {
        const QString str = "update friendRequest set isvalid=true where id =" + id;
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "update friend request state acord id failed");
            return false;
        }
        return true;
    }
    bool AddFriendDatabase::queryFriendRequestAcordName(const QString& name, QString& id) const
    {
        const QString str = "select id from friendRequest where name = '" + name + "'";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
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
    bool AddFriendDatabase::deleteExpiredFriendRequest() const
    {
        const QString str = "DELETE FROM friendRequest WHERE date('now', '-30 day' ,'localtime') >= date(CreatedTime)";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "delete expired friend request failed");
            return false;
        }
        return true;
    }
}
