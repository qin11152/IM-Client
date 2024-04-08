#include "ProfilePictureDatabase.h"

using namespace module;
namespace database
{
    ProfilePictureDatabase::ProfilePictureDatabase()
    {
        m_strId = PublicDataManager::get_const_instance().getMyId();
    }
    bool ProfilePictureDatabase::createTable() const
    {
        const QString str = "create table profileImage (id varchar(10),imagePath varchar(100),timestamp varchar(30))";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "create profile image table failed");
            return false;
        }
        return true;
    }
    bool ProfilePictureDatabase::insertProfilePathAndTimestamp(const QString& id, const QString& path, const QString& timestamp) const
    {
        const QString str = "insert into profileImage values(" + id + ",'" + path + "','" + timestamp + "')";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "insert into profile image failed,sql is:%s", str.toStdString().c_str());
            return false;
        }
        return true;
    }
    bool ProfilePictureDatabase::queryProfilePicturePath(const QString& id, QString& path) const
    {
        const QString str = "select imagePath from profileImage where id=" + id;
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
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
    bool ProfilePictureDatabase::queryIsIdExistInProfilePictureTable(const QString& id) const
    {
        const QString str = "select count(*) from profileImage where id=" + id;
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
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
    bool ProfilePictureDatabase::queryProfilePictureTimeStamp(std::unordered_map<std::string, std::string>& mapTimeStamp) const
    {
        const QString str = "select id, timestamp from profileImage";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
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
    bool ProfilePictureDatabase::updateFriendPictureTimestamp(const QString& id, const QString& newTimeStamp) const
    {
        QString str = "update profileImage set timestamp='" + newTimeStamp + "' where id=" + id;
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "update friend image timestamp%d failed", id.toInt());
            return false;
        }
        return true;
    }
    bool ProfilePictureDatabase::updateProfilePicturePath(const QString& id, const QString& newPath) const
    {
        const QString str = "update profileImage set imagePath ='" + newPath + "' where id =" + id;
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "update profile image path failed");
            return false;
        }
        return true;
    }
    bool ProfilePictureDatabase::updateProfillePicturePathAndTimeStamp(const QString& id, const QString& newPath, const QString& newTimeStamp) const
    {
        const QString str = "update profileImage set imagePath ='" + newPath + "',timestamp='" + newTimeStamp + "'where id =" + id;
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, query))
        {
            _LOG(Logcxx::Level::ERRORS, "update profile image path and time failed");
            return false;
        }
        return true;
    }
}
