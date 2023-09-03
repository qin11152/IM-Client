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
         * brief���������������id��������Ϣ��.
         * 
         * \return �ɹ����
         */
        bool createChatTable() const;
        /**
         * brief�����������¼.
         * 
         * \param TotalCount����ǰ�ܹ��ж����������¼
         * \param message��������Ϣ
         * \param time����Ϣʱ��
         * \param isSelf���Ƿ����Լ����͵�
         * \param name����������
         * \return �ɹ����
         */
        bool insertChatRecoed(int TotalCount, const QString& message, const QString& time,
        bool isSelf, const QString& name) const;
        /**
         * brief����ѯ���һ�������¼����������ʾ������б��У�.
         * 
         * \return 
         */
        QString queryFinalChatRecord() const;
        //���������¼
        /**
         * brief�����������¼.
         * 
         * \param TotalCount����ǰ����������
         * \param message��Ҫ�������Ϣ
         * \param time�������¼����ʱ��
         * \param isSelf���Ƿ��Լ����͵�
         * \return 
         */
        bool insertChatRecord(int TotalCount, const QString& message, const QString& time,bool isSelf)const;
        /**
         * brief:���ݲ�ѯһ�������������¼.
         * 
         * \param id����������id
         * \param chatMessage�����ڴ洢�����¼��vec
         * \param queryCount����Ҫ��ѯ������
         * \param beginPos�������￪ʼ�Ĳ�ѯ��Ĭ��0
         * \return �ɹ����
         */
        bool queryCertainCountChatRecordAcodId(QString id, std::vector<MyChatMessageInfo>& chatMessage, int queryCount,
            int beginPos=0)const;

    private:
        QString m_strFriendId{ "" };    //������ѵ�id
        QString m_id{ "" };     //���Լ���id
        QString m_strFriendName{ "" }; //������ѵ�����
    };
}

