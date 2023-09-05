#pragma once

#include "module/DataBaseDelegate/DatabaseOperate.h"

namespace database
{
    class LastChatDatabase
    {
    public:
        LastChatDatabase();
        virtual ~LastChatDatabase() = default;
        const LastChatDatabase& operator=(const LastChatDatabase& other) = delete;
        LastChatDatabase(const LastChatDatabase& other) = delete;

        /**
         * brief:创建lastchat表，主要用于侧边栏显示已经聊过的对象.
         * 
         * \return 成功与否
         */
        bool createTable()const;

        /**
         * brief：插入内容到上次聊天表，单个插入.
         * 
         * \param id：聊天对象的id
         * \param isGroupChat：是否为群聊
         * \return 
         */
        bool insertLastChat(const QString& id, bool isGroupChat)const;
        /**
         * brief：批量插入到上次聊天表.
         * 
         * \param order：pair中key为id，value为是否为群聊
         * \return 
         */
        bool insertLastChat(const std::vector<std::pair<QString, bool>>& order)const;

        /**
         * brief：清空上次聊天表.
         * 
         * \return 
         */
        bool clearLastChat()const;

        /**
         * brief：从数据库中查找出上次关闭时的聊天列表，用于初始化界面.
         * 
         * \param m_tmpVec：上次聊天列表
         * \return 
         */
        bool queryLastChatListFromDB(std::vector<MyLastChatFriendInfo>& m_tmpVec)const;

        /**
         * brief：删除表内相关内容.
         * 
         * \return 
         */
        bool deleteInfo()const;

    private:
        QString m_strId{ "" };
    };
}