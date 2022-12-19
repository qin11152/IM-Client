#pragma once

#include "module/stdafx.h"

#include <QStackedWidget>
#include <QQuickWidget>

class MyStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    MyStackedWidget(QWidget *parent);
    ~MyStackedWidget();
    void SwitchToChatPage(int iId);
    void SwitchToChatPageAcordIndex(int index);
    void insertToMap(int iId,QWidget* wid);
    void removeFromMap(int iId);
    QWidget* getWidAcord2Id(int iId);
    bool isWidCreate(int id);

private:
    //intΪidֵ��quickwidget�Ƕ�Ӧchatmessage qml����ָ�룬�������id�ҵ�ָ��
    std::map<int, QWidget*> m_mapIdWidgetReference;
};
