#include "MyChatMessageQuickWid.h"
#include "../DataBaseDelegate/DataBaseDelegate.h"

MyChatMessageQuickWid::MyChatMessageQuickWid(QWidget *parent)
    : QQuickWidget(parent)
{
    ui.setupUi(this);
}

MyChatMessageQuickWid::~MyChatMessageQuickWid()
{
}

//设置和当前用户的聊天记录数量
void MyChatMessageQuickWid::setRecordCount(int cnt)
{
    m_iChatRecordCount = cnt;
}

//获得当前用户聊天记录数量
int MyChatMessageQuickWid::getRecordCount() const
{
    return m_iChatRecordCount;
}

void MyChatMessageQuickWid::setRootObj()
{
    m_ptrRootObj = reinterpret_cast<QObject*>(rootObject());
}

QObject* MyChatMessageQuickWid::getRootObj() const
{
    return m_ptrRootObj;
}

void MyChatMessageQuickWid::SetUserId(int id)
{
    m_iId = id;
    //DataBaseDelegate::Instance()->QueryInitialAcordIdFromDB(id, m_strInitial);
}
