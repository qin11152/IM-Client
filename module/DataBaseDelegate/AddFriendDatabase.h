#pragma once

#include "module/DataBaseDelegate/DatabaseOperate.h"

namespace database
{
    class AddFriendDatabase
    {
    public:
        AddFriendDatabase();
        virtual ~AddFriendDatabase() = default;
        AddFriendDatabase(const AddFriendDatabase&) = delete;
        const AddFriendDatabase& operator=(const AddFriendDatabase&) = delete;

        /**
        * brief:创建表用于记录已有的好友请求.
        *
        * \return 成功与否
        */
        bool createTable()const;

        /**
         * brief：插入好友请求记录到数据库.
         * 
         * \param id：对方的id
         * \param name：对方的昵称
         * \param verifyMsg：对方发送的验证消息
         * \return 
         */
        bool insertAddFriendRequest(const QString& id, const QString& name, const QString& verifyMsg)const;

        /**
         * brief：获取好友请求信息，包括已添加和未添加.
         * 
         * \param id
         * \param addFriendInfo：vec中存储好友请求相关信息的结构体
         * \return 
         */
        bool queryAddFriendInfoFromDB(std::vector<MyAddFriendInfo>& addFriendInfo)const;
        
        /**
         * brief： 根据id把相应的好友请求置为true.
         * 
         * \param id：通过好友请求的id
         * \return 
         */
        bool updateFriendRequestStateAcordId(const QString& id)const;
        
        /**
         * brief：根据昵称去查找好友请求对应的id.
         * 
         * \param name：昵称
         * \param id：对应id
         * \return 
         */
        bool queryFriendRequestAcordName(const QString& name, QString& id)const;

        /**
         * brief：删除过期的好友请求, 这里采用30天为期限, 无论是否同意都删除.
         */
        bool deleteExpiredFriendRequest()const;

    private:
        QString m_strId{ "" };
    };


}
