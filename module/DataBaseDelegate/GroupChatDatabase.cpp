#include "GroupChatDatabase.h"

namespace database
{
    GroupChatDatabase::GroupChatDatabase(const char* strGroupId)
        :m_strGroupId(strGroupId)
    {
        m_strId = PublicDataManager::get_const_instance().getMyId();
    }
    bool GroupChatDatabase::createTable()
    {
        const QString str = "create table groupchat" + m_strGroupId + " (pos integer primary key autoincrement,message varchar(200),time varchar(20),userid integer,name varchar(20))";
        QSqlQuery query;
        if (!DataBaseOperate::get_mutable_instance().executeSql(str,query))
        {
            _LOG(Logcxx::Level::ERRORS, "create group char table failed,id:%s", m_strGroupId.toStdString().c_str());
            return false;
        }
        return true;
    }
}
