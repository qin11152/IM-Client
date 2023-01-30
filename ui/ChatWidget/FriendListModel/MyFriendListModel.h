#pragma once

#include "module/stdafx.h"

#include <QAbstractListModel>

/**
 * brief：好友列表模型使用的数据结构体.
 */
struct FriendListInfo
{
	QString m_strFriendId{ "" };	//好友id
	QString m_strFriendName{ "" };	//好友名字
	QString m_strFriendImagePath{ "" };	//好友头像路径
};

class MyFriendListModel  : public QAbstractListModel
{
	Q_OBJECT

public:
	MyFriendListModel(QObject* parent = nullptr);
	~MyFriendListModel();

	void setData(std::vector<FriendListInfo>& friendListInfo);
	void addData(const FriendListInfo& friendListInfo);

protected:
	QVariant data(const QModelIndex& index, int role) const override;
	int rowCount(QModelIndex const& parent = QModelIndex()) const override;
	QMap<int, QVariant> itemData(const QModelIndex& index)const override;
	QHash<int, QByteArray> roleNames()const override;

private:
	std::vector<FriendListInfo> m_vecFriendListInfo;
};

Q_DECLARE_METATYPE(FriendListInfo);
