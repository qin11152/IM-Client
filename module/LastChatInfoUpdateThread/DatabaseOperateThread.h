#pragma once
/////
///一个能够在子线程中操作数据库的类，当有些耗时数据库的操作需要在子线程中进行时，可以使用此类
///

#include "module/MyDefine.h"
#include <QThread>
#include <QSqlDatabase>

class DatabaseOperateThread : public QThread
{
    Q_OBJECT

public:
    explicit  DatabaseOperateThread(QObject *parent=nullptr);
    ~DatabaseOperateThread()override = default;
    DatabaseOperateThread& operator=(const DatabaseOperateThread& l) = delete;
    DatabaseOperateThread(const DatabaseOperateThread& l) = delete;
    DatabaseOperateThread& operator=(DatabaseOperateThread&& l) = delete;
    DatabaseOperateThread(DatabaseOperateThread&& l) = delete;


    void setLastChatQML(QObject* ptr) { m_ptrLastChatQML = ptr; }
    //设定线程操作的类型
    void setOperateType(const DatabaseOperateType& operateType);
    //设定当前登录用户的id
    void setCurUserId(const QString& curId = "");
    //初始化连接等操作
    void init();
    //设定最新的lastchat列表
    void setLastChatList(QStringList& lastChatList);

protected:
    void run()override;

private:
    //判断表是否存在
    bool isTableExist(const QString& tableName)const;
    //创建lastchat表
    bool createLastChat()const;
    //清空lastchat表
    bool clearLastChat() const;
    //向lastchat中插入记录
    bool insertLastChat(const QString& id)const;
    //获取最新的lastchat顺序
    void getLastChatOrder(QStringList& modelOrder)const;

private:
    QObject* m_ptrLastChatQML{ nullptr };
    //用来指定线程操作时执行哪一种操作
    DatabaseOperateType m_threadOperateType{ DatabaseOperateType::None };
    //数据库成员
    QSqlDatabase m_dataBase;
    //当前登录用户的id
    QString m_strCurrentUserId{ "" };
    //最新的lastchat列表
    QStringList m_lastChatList;
};
