#pragma once

#include "module/stdafx.h"
#include <QAbstractListModel>


class MyFriendListModel  : public QAbstractListModel
{
	Q_OBJECT

public:
	MyFriendListModel(QObject* parent = nullptr);
	~MyFriendListModel();

	void setData(std::vector<MyFriendInfoWithFirstC>& friendListInfo);
	void addData(const MyFriendInfoWithFirstC& friendListInfo);

protected:
	QVariant data(const QModelIndex& index, int role) const override;
	int rowCount(QModelIndex const& parent = QModelIndex()) const override;
	QMap<int, QVariant> itemData(const QModelIndex& index)const override;
	QHash<int, QByteArray> roleNames()const override;

private:
	std::vector<MyFriendInfoWithFirstC> m_vecFriendListInfo;
};
