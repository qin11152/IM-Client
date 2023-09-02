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
        void init();
        bool executeSql(const QString& cmd,QSqlRecord& record);

        const DataBaseOperate& operator=(const DataBaseOperate&) = delete;
        DataBaseOperate(const DataBaseOperate&) = delete;

    private:
        bool m_bConnectedState{ false };
        QSqlDatabase m_dataBase;
    };
}