#pragma once

#include "module/DataBaseDelegate/DatabaseOperate.h"

namespace database
{
    class ProfilePictureDatabase
    {
    public:
        ProfilePictureDatabase();
        virtual ~ProfilePictureDatabase() = default;
        const ProfilePictureDatabase& operator=(const ProfilePictureDatabase& other) = delete;
        ProfilePictureDatabase(const ProfilePictureDatabase& other) = delete;

        /**
         * brief：创建好友头像表.
         * 
         * \return 
         */
        bool createTable()const;

        /**
         * brief：插入好友头像路径和时间戳到表中.
         * 
         * \param id：好友id
         * \param path：头像路径
         * \param timestamp：头像时间戳，对比头像是否变化用
         * \return 
         */
        bool insertProfilePathAndTimestamp(const QString& id, const QString& path, const QString& timestamp)const;

        /**
         * brief：根据id查询好友头像路径.
         * 
         * \param id：好友id
         * \param path：返回的路径
         * \return 
         */
        bool queryProfilePicturePath(const QString& id, QString& path)const;

        /**
         * brief：查询头像表中这个id是否存在.
         * 
         * \param id：要查询的id
         * \return 
         */
        bool queryIsIdExistInProfilePictureTable(const QString& id)const;

        /**
         * brief：获取表中所有好友的头像时间戳.
         * 
         * \param mapTimeStamp：key：id，value：时间戳
         * \return 
         */
        bool queryProfilePictureTimeStamp(std::unordered_map<std::string, std::string>& mapTimeStamp)const;

        /**
         * brief：更新好友头像时间戳.
         * 
         * \param id：好友id
         * \param newTimeStamp：新的时间戳
         * \return 
         */
        bool updateFriendPictureTimestamp(const QString& id, const QString& newTimeStamp)const;
        
        /**
         * brief：更新好友头像路径.
         * 
         * \param id：好友id
         * \param newPath：新的路径
         * \return 
         */
        bool updateProfilePicturePath(const QString& id, const QString& newPath)const;
        
        /**
         * brief：更新头像路径和时间戳.
         * 
         * \param id：要更新的id
         * \param newPath：新路径
         * \param newTimeStamp：新时间戳
         * \return 
         */
        bool updateProfillePicturePathAndTimeStamp(const QString& id, const QString& newPath, const QString& newTimeStamp)const;

    private:
        QString m_strId{ "" };
    };
}
