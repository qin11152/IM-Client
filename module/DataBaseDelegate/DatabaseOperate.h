#pragma once

#include "module/stdafx.h"
#include "boost/serialization/serialization.hpp"

#include <QDir>
#include <QObject>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <qsqlrecord.h>

namespace database
{
    class DataBaseOperate :public QObject, public serialization::singleton<DataBaseOperate>
    {
        Q_OBJECT
    public:
        virtual ~DataBaseOperate();
        DataBaseOperate();
        void closeDB();
        bool init();
        void initTables();
        /**
         * brief:�������ݿ�����ָ��sql��䣬ֻ�������߳��е��ã����ܿ��̵߳���.
         * 
         * \param cmd:��Ҫָ��������
         * \param record:ִ�н��
         * \return ��ִ�гɹ����
         */
        bool executeSql(const QString& cmd,QSqlQuery& query);

        /**
         * brief��ֻ����ִ�н���������ȡ���ݿ��еĲ�ѯֵʱ����.
         * 
         * \param cmd
         * \return 
         */
        bool executeSqlWithoutReturn(const QString& cmd);

        /**
         * brief:�鿴ĳ�����Ƿ����.
         * 
         * \param tableName:������
         * \return �Ƿ����
         */
        bool isTableExist(const QString& tableName)const;

        const DataBaseOperate& operator=(const DataBaseOperate&) = delete;
        DataBaseOperate(const DataBaseOperate&) = delete;

    private:
        bool m_bConnectedState{ false };
        QSqlDatabase m_dataBase;
        //���浱ǰ�߳�id
        std::thread::id m_threadId;
    };

    class DatabaseOperateForQml : public QObject
    {
    public:
        using SingletonPtr = std::shared_ptr<DatabaseOperateForQml>;
        //����ģʽ��Ψһ��ȡ��ָ��ķ�����ʹ��������ָ�룬�Զ��ڴ����
        static SingletonPtr instance();
        virtual ~DatabaseOperateForQml() = default;

        int getChatRecordCountFromDB(const QString& strId);

    private:
        static std::mutex m_mutex;                  //������֤�̰߳�ȫ
        static SingletonPtr m_SingletonPtr;         //���������ָ��
        DatabaseOperateForQml(QObject* parent=nullptr);

    };
}

