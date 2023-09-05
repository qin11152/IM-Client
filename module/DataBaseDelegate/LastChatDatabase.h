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
         * brief:����lastchat����Ҫ���ڲ������ʾ�Ѿ��Ĺ��Ķ���.
         * 
         * \return �ɹ����
         */
        bool createTable()const;

        /**
         * brief���������ݵ��ϴ��������������.
         * 
         * \param id����������id
         * \param isGroupChat���Ƿ�ΪȺ��
         * \return 
         */
        bool insertLastChat(const QString& id, bool isGroupChat)const;
        /**
         * brief���������뵽�ϴ������.
         * 
         * \param order��pair��keyΪid��valueΪ�Ƿ�ΪȺ��
         * \return 
         */
        bool insertLastChat(const std::vector<std::pair<QString, bool>>& order)const;

        /**
         * brief������ϴ������.
         * 
         * \return 
         */
        bool clearLastChat()const;

        /**
         * brief�������ݿ��в��ҳ��ϴιر�ʱ�������б����ڳ�ʼ������.
         * 
         * \param m_tmpVec���ϴ������б�
         * \return 
         */
        bool queryLastChatListFromDB(std::vector<MyLastChatFriendInfo>& m_tmpVec)const;

        /**
         * brief��ɾ�������������.
         * 
         * \return 
         */
        bool deleteInfo()const;

    private:
        QString m_strId{ "" };
    };
}