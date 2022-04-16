/************************************************************************/
/* 聊天界面逻辑部门管理类，相关逻辑操作均定义在此                                                                   
/************************************************************************/
#pragma once

#include "../MyDefine.h"
#include <QObject>
#include <vector>

class ChatWidgetManager : public QObject
{
    Q_OBJECT

public:
    ChatWidgetManager(QObject *parent);
    ~ChatWidgetManager();

    //从服务端获取当前用户的好友列表
    void getFriendListFromServer(int iId);

private:
    //存储好友信息，带首字母
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;
};
