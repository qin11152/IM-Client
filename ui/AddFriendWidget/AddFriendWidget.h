#pragma once

#include "ui_AddFriendWidget.h"
#include "ui/AddFriendWidget/AddFriendModel.h"
#include "ui/AddFriendWidget/AddFriendDelegate.h"
#include "ui/PublicGUI/NotifyLabel/NotifyLabel.h"

#include <QWidget>

class AddFriendWidget : public QWidget
{
    Q_OBJECT

public:
    AddFriendWidget(QWidget *parent = nullptr);
    ~AddFriendWidget();

    void setData(std::vector<AddFriendInfo>& vecFriendInfo);

    void updateModelValidState(const QString& id, bool validState);

    void updateModelImagePath(const QString& id, const QString& path);
private slots:
    /**
     * brief：当发送按钮点击后.
     */
    void onSignalAddBtnClicked();

    void onSignalAgreeClicked(const QString& id);

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

    /**
     * brief：初始化数据.
     * 
     */
    void initData();

private:
    Ui::AddFriendWidgetClass ui;
    NotifyLabel* m_notifyLabel{ nullptr };
    AddFriendModel* m_ptrModel{ nullptr };
    AddFriendDelegate* m_ptrDelegate{ nullptr };
};
