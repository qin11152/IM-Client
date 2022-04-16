/************************************************************************/
/* ��������߼����Ź����࣬����߼������������ڴ�                                                                   
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

    //�ӷ���˻�ȡ��ǰ�û��ĺ����б�
    void getFriendListFromServer(int iId);

private:
    //�洢������Ϣ��������ĸ
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;
};
