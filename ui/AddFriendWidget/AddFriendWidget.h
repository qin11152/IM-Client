#pragma once

#include "ui_AddFriendWidget.h"
#include "ui/PublicGUI/NotifyLabel/NotifyLabel.h"

#include <QWidget>

class AddFriendWidget : public QWidget
{
    Q_OBJECT

public:
    AddFriendWidget(QWidget *parent = nullptr);
    ~AddFriendWidget();

private slots:
    /**
     * brief�������Ͱ�ť�����.
     */
    void onSignalAddBtnClicked();

protected:
    void closeEvent(QCloseEvent* event)override;

private:
    /**
     * brief����ʼ��������ز���.
     * 
     */
    void initUI();
    
    /**
     * brief����ʼ���źŲ�����.
     * 
     */
    void initConnect();

private:
    Ui::AddFriendWidgetClass ui;
    NotifyLabel* m_notifyLabel{ nullptr };
};
