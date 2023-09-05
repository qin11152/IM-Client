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
         * brief����������ͷ���.
         * 
         * \return 
         */
        bool createTable()const;

        /**
         * brief���������ͷ��·����ʱ���������.
         * 
         * \param id������id
         * \param path��ͷ��·��
         * \param timestamp��ͷ��ʱ������Ա�ͷ���Ƿ�仯��
         * \return 
         */
        bool insertProfilePathAndTimestamp(const QString& id, const QString& path, const QString& timestamp)const;

        /**
         * brief������id��ѯ����ͷ��·��.
         * 
         * \param id������id
         * \param path�����ص�·��
         * \return 
         */
        bool queryProfilePicturePath(const QString& id, QString& path)const;

        /**
         * brief����ѯͷ��������id�Ƿ����.
         * 
         * \param id��Ҫ��ѯ��id
         * \return 
         */
        bool queryIsIdExistInProfilePictureTable(const QString& id)const;

        /**
         * brief����ȡ�������к��ѵ�ͷ��ʱ���.
         * 
         * \param mapTimeStamp��key��id��value��ʱ���
         * \return 
         */
        bool queryProfilePictureTimeStamp(std::unordered_map<std::string, std::string>& mapTimeStamp)const;

        /**
         * brief�����º���ͷ��ʱ���.
         * 
         * \param id������id
         * \param newTimeStamp���µ�ʱ���
         * \return 
         */
        bool updateFriendPictureTimestamp(const QString& id, const QString& newTimeStamp)const;
        
        /**
         * brief�����º���ͷ��·��.
         * 
         * \param id������id
         * \param newPath���µ�·��
         * \return 
         */
        bool updateProfilePicturePath(const QString& id, const QString& newPath)const;
        
        /**
         * brief������ͷ��·����ʱ���.
         * 
         * \param id��Ҫ���µ�id
         * \param newPath����·��
         * \param newTimeStamp����ʱ���
         * \return 
         */
        bool updateProfillePicturePathAndTimeStamp(const QString& id, const QString& newPath, const QString& newTimeStamp)const;

    private:
        QString m_strId{ "" };
    };
}
