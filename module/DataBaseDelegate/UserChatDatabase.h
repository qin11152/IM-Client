#pragma once

#include "DatabaseOperate.h"

namespace database
{
    class UserChatDatabase
    {
    public:
        UserChatDatabase(const char* userId);
        ~UserChatDatabase()=default;
        bool createChatTable();
        bool insertChatRecoed(int TotalCount, const QString& message, const QString& time,
            bool isSelf, const QString& name);

    private:
        QString m_strfriendId{ "" };    //这个好友的id
        QSqlRecord m_record;
    };
}

