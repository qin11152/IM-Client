#include "UserChatDatabase.h"

namespace database
{
    UserChatDatabase::UserChatDatabase(const char* userId):
        m_strfriendId(userId)
    {
    }

    bool UserChatDatabase::createChatTable()
    {
        //每个用户都有一个自己的库，在自己的库中和每个人的聊天记录是一个表
        QString str = "CREATE TABLE chatrecord" + m_strfriendId + "(pos INTEGER PRIMARY KEY AUTOINCREMENT, message VARCHAR(50), time VARCHAR(20), isself  BOOL, name VARCHAR(30));";
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, m_record));
        {
            return false;
        }
        return true;
    }
    bool UserChatDatabase::insertChatRecoed(int TotalCount, const QString& message, const QString& time, bool isSelf, const QString& name)
    {
        auto strIsSelf = isSelf ? "true" : "false";
        const QString str = "insert into chatrecord" + PublicDataManager::get_mutable_instance().getMyId() + " values (" + QString::number(TotalCount + 1) + ",\"" + message + "\",\"" + time + "\"," + strIsSelf + ",\"" + name + "\")";
        if (!DataBaseOperate::get_mutable_instance().executeSql(str, m_record))
        {
            _LOG(Logcxx::Level::ERRORS, "insert chat record failed");
            return false;
        }
        return true;
    }
}
