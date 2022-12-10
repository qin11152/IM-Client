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
     * brief：当发送按钮点击后.
     */
    void onSignalAddBtnClicked();

protected:
    void closeEvent(QCloseEvent* event)override;

private:
    /**
     * brief：初始化界面相关操作.
     * 
     */
    void initUI();
    
    /**
     * brief：初始化信号槽连接.
     * 
     */
    void initConnect();

private:
    Ui::AddFriendWidgetClass ui;
    NotifyLabel* m_notifyLabel{ nullptr };
};
