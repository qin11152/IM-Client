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
    explicit  DatabaseOperateThread(QObject *parent=nullptr);
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
    //��ʼ�����ӵȲ���
    void init();
    //�趨���µ�lastchat�б�
    void setLastChatList(QStringList& lastChatList);

protected:
    void run()override;

private:
    //�жϱ��Ƿ����
    bool isTableExist(const QString& tableName)const;
    //����lastchat��
    bool createLastChat()const;
    //���lastchat��
    bool clearLastChat() const;
    //��lastchat�в����¼
    bool insertLastChat(const QString& id)const;
    //��ȡ���µ�lastchat˳��
    void getLastChatOrder(QStringList& modelOrder)const;

private:
    QObject* m_ptrLastChatQML{ nullptr };
    //����ָ���̲߳���ʱִ����һ�ֲ���
    DatabaseOperateType m_threadOperateType{ DatabaseOperateType::None };
    //���ݿ��Ա
    QSqlDatabase m_dataBase;
    //��ǰ��¼�û���id
    QString m_strCurrentUserId{ "" };
    //���µ�lastchat�б�
    QStringList m_lastChatList;
};
