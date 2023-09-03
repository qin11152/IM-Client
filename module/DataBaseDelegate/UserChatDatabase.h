#pragma once

#include "DatabaseOperate.h"

namespace database
{
    class UserChatDatabase
    {
    public:
        UserChatDatabase(const char* friendId, const char* friendName);
        ~UserChatDatabase()=default;

        /**
         * brief：创建和这个好友id的聊天信息表.
         * 
         * \return 成功与否
         */
        bool createChatTable() const;
        /**
         * brief：插入聊天记录.
         * 
         * \param TotalCount：当前总共有多少条聊天记录
         * \param message：聊天消息
         * \param time：消息时间
         * \param isSelf：是否是自己发送的
         * \param name：好友名字
         * \return 成功与否
         */
        bool insertChatRecoed(int TotalCount, const QString& message, const QString& time,
        bool isSelf, const QString& name) const;
        /**
         * brief：查询最后一条聊天记录（多用于显示在左侧列表中）.
         * 
         * \return 
         */
        QString queryFinalChatRecord() const;
        //插入聊天记录
        /**
         * brief：插入聊天记录.
         * 
         * \param TotalCount：当前总聊天数量
         * \param message：要插入的消息
         * \param time：聊天记录产生时间
         * \param isSelf：是否自己发送的
         * \return 
         */
        bool insertChatRecord(int TotalCount, const QString& message, const QString& time,bool isSelf)const;
        /**
         * brief:根据查询一定数量的聊天记录.
         * 
         * \param id：聊天对象的id
         * \param chatMessage：用于存储聊天记录的vec
         * \param queryCount：需要查询的数量
         * \param beginPos：从哪里开始的查询，默认0
         * \return 成功与否
         */
        bool queryCertainCountChatRecordAcodId(QString id, std::vector<MyChatMessageInfo>& chatMessage, int queryCount,
            int beginPos=0)const;

    private:
        QString m_strFriendId{ "" };    //这个好友的id
        QString m_id{ "" };     //我自己的id
        QString m_strFriendName{ "" }; //这个好友的名字
    };
}

