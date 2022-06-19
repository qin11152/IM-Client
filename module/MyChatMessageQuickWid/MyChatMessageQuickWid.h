/************************************************************************/
/* ����Ϊ��������qml�����࣬���Ա��浱ǰ�������¼������
/************************************************************************/

#pragma once

#include <QQuickWidget>
#include <QQuickView>
#include <atomic>
#include "ui_MyChatMessageQuickWid.h"

class MyChatMessageQuickWid : public QQuickWidget
{
    Q_OBJECT

public:
    MyChatMessageQuickWid(QWidget *parent = Q_NULLPTR);
    ~MyChatMessageQuickWid();
    //���������¼����
    void addRecordCount(int cnt);
    //��ȡ�����¼����
    int getRecordCount()const;
    //���������¼������
    void setTotalRecordCount(int cnt);
    //��������¼������
    int getTotalRecordCount()const;
    //���ø�����
    void setRootObj();
    //��ȡ��������QML���н���
    QObject* getRootObj()const;
    //����id�Ľ�ڣ��ⲿ��֪��������Ӧ���û�id�Ƕ���
    void SetUserId(QString id);
    //��ȡid�ӿ�
    QString GetUserId()const { return m_strId; }
    //��������ĸ
    void setInitial(QString strInitial) { m_strInitial = strInitial; }
    //��ȡ����ĸ
    QString GetInitial()const { return m_strInitial; }
    //��ȡ�û�����
    QString getUserName()const { return m_strUserName; }
    //�����û�����
    void setUserName(QString strUserName) { m_strUserName = strUserName; setInitial(m_strUserName.mid(0, 1)); }

private:
    Ui::MyChatMessageQuickWid ui;
    //�����¼������
    std::atomic<int> m_iChatRecordCount{ 0 };
    //�����¼������
    std::atomic<int> m_iTotalRecordCount{ 0 };
    //���������ں�qml���н���
    QObject* m_ptrRootObj{ nullptr };
    //�û���id
    QString m_strId{ "" };
    //�û�������ĸ
    QString m_strInitial{ "" };
    //�û�������
    QString m_strUserName{ "" };

};
