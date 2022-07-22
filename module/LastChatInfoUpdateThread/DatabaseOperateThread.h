#pragma once
/////
///һ���ܹ������߳��в������ݿ���࣬����Щ��ʱ���ݿ�Ĳ�����Ҫ�����߳��н���ʱ������ʹ�ô���
///

#include "module/MyDefine.h"
#include <QThread>
#include <QSqlDatabase>

class DatabaseOperateThread : public QThread
{
    Q_OBJECT

public:
    explicit  DatabaseOperateThread(QObject *parent);
    ~DatabaseOperateThread()override = default;
    DatabaseOperateThread& operator=(const DatabaseOperateThread& l) = delete;
    DatabaseOperateThread(const DatabaseOperateThread& l) = delete;
    DatabaseOperateThread& operator=(DatabaseOperateThread&& l) = delete;
    DatabaseOperateThread(DatabaseOperateThread&& l) = delete;


    void setLastChatQML(QObject* ptr) { m_ptrLastChatQML = ptr; }
    //�趨�̲߳���������
    void setOperateType(const DatabaseOperateType& operateType);
    //�趨��ǰ��¼�û���id
    void setCurUserId(const QString& curId = "");

protected:
    void run()override;

private:
    //��ʼ�����ӵȲ���
    void init();
    //�ж��̲߳��������ݿ��У���ǰ�û�lastchat���Ƿ񴴽�
    bool isLastChatExist();
    //��ǰ�û���lastchat�в�������
    void insertIntoLastChat();

private:
    QObject* m_ptrLastChatQML{ nullptr };
    //����ָ���̲߳���ʱִ����һ�ֲ���
    DatabaseOperateType m_threadOperateType{ DatabaseOperateType::None };
    //���ݿ��Ա
    QSqlDatabase m_dataBase;
    //��ǰ��¼�û���id
    QString m_strCurrentUserId{ "" };
};
