#pragma once

#include "module/DataBaseDelegate/DatabaseOperate.h"

namespace database
{
    class GroupChatDatabase
    {
    public:
        GroupChatDatabase(const char* strGroupId);
        virtual ~GroupChatDatabase() = default;
        const GroupChatDatabase& operator=(const GroupChatDatabase&) = delete;
        GroupChatDatabase(const GroupChatDatabase&) = delete;
       
        /**
         * brief：创建某个群聊id对应的聊天信息表.
         *
         * \return 成功与否
         */
        bool createTable();

    private:
        QString m_strId{ "" };
        QString m_strGroupId{ "" };
    };
}
