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

//���ú͵�ǰ�û��������¼����
void MyChatMessageQuickWid::setRecordCount(int cnt)
{
    m_iChatRecordCount = cnt;
}

//��õ�ǰ�û������¼����
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
