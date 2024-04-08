#pragma once

#include "ui_StartGroupChatWidget.h"
#include "ui/StartGroupChatWidget/AddedGroupFriendMVC/AddedFriendModel.h"
#include "ui/StartGroupChatWidget/AddedGroupFriendMVC/AddedFriendDelegate.h"
#include "ui/StartGroupChatWidget/ChooseFriendGroupMVC/ChooseFriendModel.h"
#include "ui/StartGroupChatWidget/ChooseFriendGroupMVC/ChooseFriendDelegate.h"
#include "ui/StartGroupChatWidget/ChooseFriendGroupMVC/ChooseFriendSortModel.h"

#include <QWidget>

namespace wechat
{
    class StartGroupChatWidget : public QWidget
    {
        Q_OBJECT

    public:
        StartGroupChatWidget(QWidget* parent = nullptr);
        ~StartGroupChatWidget();

        void setModelData(std::vector<MyFriendInfoForStartGroupChat>& friendInfo);

    private slots:

        void onSignalFinishClicked();
        void onSignalCancelClicked();

        /**
         * brief:选中某个好友之后的响应.
         *
         * \param id:选择的好友的id
         * \param newState:新的状态
         */
        void onSignalUpdateSelectedState(const QString& id, bool newState);

        void onSignalSearchChanged(const QString& searchMsg);

        /**
         * brief:已添加页面删除了某个人.
         *
         * \param id:删除的人的id
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
}
