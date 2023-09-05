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
        * brief:���������ڼ�¼���еĺ�������.
        *
        * \return �ɹ����
        */
        bool createTable()const;

        /**
         * brief��������������¼�����ݿ�.
         * 
         * \param id���Է���id
         * \param name���Է����ǳ�
         * \param verifyMsg���Է����͵���֤��Ϣ
         * \return 
         */
        bool insertAddFriendRequest(const QString& id, const QString& name, const QString& verifyMsg)const;

        /**
         * brief����ȡ����������Ϣ����������Ӻ�δ���.
         * 
         * \param id
         * \param addFriendInfo��vec�д洢�������������Ϣ�Ľṹ��
         * \return 
         */
        bool queryAddFriendInfoFromDB(std::vector<MyAddFriendInfo>& addFriendInfo)const;
        
        /**
         * brief�� ����id����Ӧ�ĺ���������Ϊtrue.
         * 
         * \param id��ͨ�����������id
         * \return 
         */
        bool updateFriendRequestStateAcordId(const QString& id)const;
        
        /**
         * brief�������ǳ�ȥ���Һ��������Ӧ��id.
         * 
         * \param name���ǳ�
         * \param id����Ӧid
         * \return 
         */
        bool queryFriendRequestAcordName(const QString& name, QString& id)const;

        /**
         * brief��ɾ�����ڵĺ�������, �������30��Ϊ����, �����Ƿ�ͬ�ⶼɾ��.
         */
        bool deleteExpiredFriendRequest()const;

    private:
        QString m_strId{ "" };
    };


}
