#pragma once

#include "ui_StartGroupChatWidget.h"
#include "ui/StartGroupChatWidget/ChooseFriendGroupMVC/ChooseFriendModel.h"	
#include "ui/StartGroupChatWidget/ChooseFriendGroupMVC/ChooseFriendDelegate.h"
#include "ui/StartGroupChatWidget/ChooseFriendGroupMVC/ChooseFriendSortModel.h"

#include <QWidget>


class StartGroupChatWidget : public QWidget
{
	Q_OBJECT

public:
	StartGroupChatWidget(QWidget *parent = nullptr);
	~StartGroupChatWidget();

	void setModelData(std::vector<MyFriendInfoWithFirstC>& friendInfo);

private slots:

	void onSignalFinishClicked();
	void onSignalCancelClicked();

private:
	void initUI();
	void initData();
	void initConnect();

private:
	Ui::StartGroupChatWidgetClass ui;

	ChooseFriendModel* m_ptrChooseFriendModel;
	ChooseFriendDelegate* m_ptrChooseFriendDelegate;
};
