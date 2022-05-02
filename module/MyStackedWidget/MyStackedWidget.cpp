#include "MyStackedWidget.h"

MyStackedWidget::MyStackedWidget(QWidget *parent)
    : QStackedWidget(parent)
{
}

MyStackedWidget::~MyStackedWidget()
{
}

void MyStackedWidget::SwitchToChatPage(int iId)
{
    int index = indexOf(m_mapIdWidgetReference[iId]);
    setCurrentIndex(index);
}

//************************************
// Method:    SwitchToChatPageAcordIndex
// FullName:  MyStackedWidget::SwitchToChatPageAcordIndex
// Access:    public 
// Returns:   void
// Qualifier: 根据stackedwidget的index显示相应的界面
// Parameter: int index
//************************************
void MyStackedWidget::SwitchToChatPageAcordIndex(int index)
{
    setCurrentIndex(index);
}

void MyStackedWidget::insertToMap(int iId,QWidget* wid)
{
    m_mapIdWidgetReference[iId] = wid;
}

void MyStackedWidget::removeFromMap(int iId)
{
    m_mapIdWidgetReference.erase(iId);
}

QWidget* MyStackedWidget::getWidAcord2Id(int iId)
{
    return m_mapIdWidgetReference[iId];
}

bool MyStackedWidget::isWidCreate(int id)
{
    if (m_mapIdWidgetReference.find(id) != m_mapIdWidgetReference.end())
    {
        return true;
    }
    return false;
}
