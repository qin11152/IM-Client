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
void MyChatMessageQuickWid::addTotalAndCurrentRecordCount(int cnt)
{
	m_iChatRecordCount += cnt;
	m_iTotalRecordCount += cnt;
}

void MyChatMessageQuickWid::addCurrentRecordCount(int cnt)
{
	m_iChatRecordCount += cnt;
}

void MyChatMessageQuickWid::setRecordCount(int cnt)
{
	m_iChatRecordCount = cnt;
}

//获得当前用户聊天记录数量
int MyChatMessageQuickWid::getRecordCount() const
{
	return m_iChatRecordCount;
}

void MyChatMessageQuickWid::setTotalRecordCount(int cnt)
{
	m_iTotalRecordCount = cnt;
}

int MyChatMessageQuickWid::getTotalRecordCount() const
{
	return m_iTotalRecordCount;
}

void MyChatMessageQuickWid::setRootObj()
{
	m_ptrRootObj = reinterpret_cast<QObject*>(rootObject());
}

QObject* MyChatMessageQuickWid::getRootObj() const
{
	return m_ptrRootObj;
}

void MyChatMessageQuickWid::SetUserId(QString id)
{
	m_strId = id;
	//DataBaseDelegate::Instance()->queryInitialAcordIdFromDB(id, m_strInitial);
}

void MyChatMessageQuickWid::focusInEvent(QFocusEvent* event)
{
	emit signalFocusChanged(true);
}
