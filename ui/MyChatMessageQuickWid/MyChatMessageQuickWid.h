﻿/************************************************************************/
/* 该类为聊天界面的qml容器类，可以保存当前的聊天记录数量等
/************************************************************************/

#pragma once

#include "module/stdafx.h"
#include "ui_MyChatMessageQuickWid.h"

#include <QQuickWidget>
#include <QQuickView>

namespace wechat
{
    class MyChatMessageQuickWid : public QQuickWidget
    {
        Q_OBJECT

    public:
        MyChatMessageQuickWid(QWidget* parent = Q_NULLPTR);
        ~MyChatMessageQuickWid();
        //增加页面中记录的聊天记录和总聊天记录的数量
        void addTotalAndCurrentRecordCount(int cnt);
        //增加页面中记录的聊天记录数量
        void addCurrentRecordCount(int cnt);
        //修改页面中已经加载了的聊天记录的数量
        void setRecordCount(int cnt);
        //获取页面中记录的的聊天记录数量
        int getRecordCount()const;
        //设置聊天记录总数量
        void setTotalRecordCount(int cnt);
        //获得聊天记录总数量
        int getTotalRecordCount()const;
        //修改未读聊天记录数量
        void setUnreadMsgCnt() { m_iUnreadMsgCnt++; }
        //获取未读聊天记录数量
        int getUnreadMsgCnt()const { return m_iUnreadMsgCnt; }
        //未读消息置0
        void clearUnreadMsgCnt() { m_iUnreadMsgCnt = 0; }
        //设置根对象
        void setRootObj();
        //获取根对象与QML进行交互
        QObject* getRootObj()const;
        //设置id的借口，外部告知这个界面对应的用户id是多少
        void SetUserId(QString id);
        //获取id接口
        QString GetUserId()const { return m_strId; }
        //设置首字母
        void setInitial(QString strInitial) { m_strInitial = strInitial; }
        //获取首字母
        QString GetInitial()const { return m_strInitial; }
        //获取用户名称
        QString getUserName()const { return m_strUserName; }
        //设置用户名称
        void setUserName(QString strUserName) { m_strUserName = strUserName; setInitial(m_strUserName.mid(0, 1)); }

        void setGroupId(QString strGroupId) { m_strGroupId = strGroupId; }

        QString getGroupId()const { return m_strGroupId; }

        void setGroupName(const QString& groupName) { m_strGroupName = groupName; }

        QString getGroupName()const { return m_strGroupName; }

    signals:
        void signalFocusChanged(bool bFocus);

    protected:
        void focusInEvent(QFocusEvent* event)override;

    private:
        Ui::MyChatMessageQuickWid ui;
        //当前页面中加载聊天记录的数量
        std::atomic<int> m_iChatRecordCount{ 0 };
        //聊天记录总数量
        std::atomic<int> m_iTotalRecordCount{ 0 };
        //根对象，用于和qml进行交互
        QObject* m_ptrRootObj{ nullptr };
        //用户的id
        QString m_strId{ "-1" };
        //群组的id
        QString m_strGroupId{ "-1" };
        //用户的首字母
        QString m_strInitial{ "" };
        //用户的名称
        QString m_strUserName{ "" };
        //群聊的名称
        QString m_strGroupName{ "" };
        //未读消息数量
        int m_iUnreadMsgCnt{ 0 };
        //是否为群聊
        bool m_bIsGroupChat{ false };
    };
}
