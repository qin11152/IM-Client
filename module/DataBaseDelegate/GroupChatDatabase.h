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
         * brief������ĳ��Ⱥ��id��Ӧ��������Ϣ��.
         *
         * \return �ɹ����
         */
        bool createTable();

    private:
        QString m_strId{ "" };
        QString m_strGroupId{ "" };
    };
}
