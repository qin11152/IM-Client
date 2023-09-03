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
         * brief:调用数据库连接指定sql语句，只能在主线程中调用，不能跨线程调用.
         * 
         * \param cmd:需要指定的命令
         * \param record:执行结果
         * \return ：执行成功与否
         */
        bool executeSql(const QString& cmd,QSqlQuery& query);

        /**
         * brief:查看某个表是否存在.
         * 
         * \param tableName:表名称
         * \return 是否存在
         */
        bool isTableExist(const QString& tableName)const;

        const DataBaseOperate& operator=(const DataBaseOperate&) = delete;
        DataBaseOperate(const DataBaseOperate&) = delete;

    private:
        bool m_bConnectedState{ false };
        QSqlDatabase m_dataBase;
        //保存当前线程id
        std::thread::id m_threadId;
    };
}