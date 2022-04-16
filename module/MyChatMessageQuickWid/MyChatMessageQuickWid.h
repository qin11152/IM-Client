/************************************************************************/
/* 该类为聊天界面的qml容器类，可以保存当前的聊天记录数量等
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
    //设置聊天记录数量
    void setRecordCount(int cnt);
    //获取聊天记录数量
    int getRecordCount()const;
    //设置根对象
    void setRootObj();
    //获取根对象与QML进行交互
    QObject* getRootObj()const;
    //设置id的借口，外部告知这个界面对应的用户id是多少
    void SetUserId(int id);
    //获取id接口
    int GetUserId()const { return m_iId; }
    //设置首字母
    void setInitial(QString strInitial) { m_strInitial = strInitial; }
    //获取首字母
    QString GetInitial()const { return m_strInitial; }
    //获取用户名称
    QString getUserName()const { return m_strUserName; }
    //设置用户名称
    void setUserName(QString strUserName) { m_strUserName = strUserName; }

private:
    Ui::MyChatMessageQuickWid ui;
    //聊天记录的数量
    std::atomic<int> m_iChatRecordCount{ 0 };
    //根对象，用于和qml进行交互
    QObject* m_ptrRootObj{ nullptr };
    //用户的id
    int m_iId{ -1 };
    //用户的首字母
    QString m_strInitial{ "" };
    //用户的名称
    QString m_strUserName{ "" };

};
