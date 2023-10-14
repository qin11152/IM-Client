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
         * brief:调用数据库连接指定sql语句，只能在主线程中调用，不能跨线程调用.
         * 
         * \param cmd:需要指定的命令
         * \param record:执行结果
         * \return ：执行成功与否
         */
        bool executeSql(const QString& cmd,QSqlQuery& query);

        /**
         * brief：只关心执行结果，无需获取数据库中的查询值时调用.
         * 
         * \param cmd
         * \return 
         */
        bool executeSqlWithoutReturn(const QString& cmd);

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

    class DatabaseOperateForQml : public QObject
    {
    public:
        using SingletonPtr = std::shared_ptr<DatabaseOperateForQml>;
        //单例模式，唯一获取其指针的方法，使用了智能指针，自动内存管理
        static SingletonPtr instance();
        virtual ~DatabaseOperateForQml() = default;

        int getChatRecordCountFromDB(const QString& strId);

    private:
        static std::mutex m_mutex;                  //锁，保证线程安全
        static SingletonPtr m_SingletonPtr;         //该类的智能指针
        DatabaseOperateForQml(QObject* parent=nullptr);

    };
}

