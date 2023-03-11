#pragma once

#include "ui_StartGroupChatWidget.h"
#include "ui/StartGroupChatWidget/AddedGroupFriendMVC/AddedFriendModel.h"
#include "ui/StartGroupChatWidget/AddedGroupFriendMVC/AddedFriendDelegate.h"
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

	void setModelData(std::vector<MyFriendInfoForStartGroupChat>& friendInfo);

private slots:

	void onSignalFinishClicked();
	void onSignalCancelClicked();

	/**
	 * brief:ѡ��ĳ������֮�����Ӧ.
	 * 
	 * \param id:ѡ��ĺ��ѵ�id
	 * \param newState:�µ�״̬
	 */
	void onSignalUpdateSelectedState(const QString& id,bool newState);

	void onSignalSearchChanged(const QString& searchMsg);

	/**
	 * brief:�����ҳ��ɾ����ĳ����.
	 * 
	 * \param id:ɾ�����˵�id
	 */
	void onSignalAddedDeleted(const QString& id);

private:
	void initUI();
	void initData();
	void initConnect();

private:
	Ui::StartGroupChatWidgetClass ui;

	ChooseFriendModel* m_ptrChooseFriendModel{ nullptr };
	ChooseFriendDelegate* m_ptrChooseFriendDelegate{ nullptr };
	ChooseFriendSortModel* m_ptrChooseFriendSortModel{ nullptr };

	AddedFriendModel* m_ptrAddFriendModel{ nullptr };
	AddedFriendDelegate* m_ptrAddFriendDelegate{ nullptr };
};
