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
        /**
         * brief:�������ݿ�����ָ��sql��䣬ֻ�������߳��е��ã����ܿ��̵߳���.
         * 
         * \param cmd:��Ҫָ��������
         * \param record:ִ�н��
         * \return ��ִ�гɹ����
         */
        bool executeSql(const QString& cmd,QSqlQuery& query);

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
}