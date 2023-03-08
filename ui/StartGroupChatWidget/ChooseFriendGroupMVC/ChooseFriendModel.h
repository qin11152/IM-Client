#pragma once

#include "module/stdafx.h"

#include <QAbstractListModel>

class ChooseFriendModel  : public QAbstractListModel
{
	Q_OBJECT

public:
	ChooseFriendModel(QObject* parent = nullptr);
	~ChooseFriendModel();

	void setData(std::vector<MyFriendInfoWithFirstC>& friendInfo);

protected:
	QVariant data(QModelIndex const& index, int role) const override;
	int rowCount(QModelIndex const& parent = QModelIndex()) const override;
	QMap<int, QVariant> itemData(const QModelIndex& index)const override;
	QHash<int, QByteArray> roleNames()const override;

private:
	std::vector<MyFriendInfoWithFirstC> m_vecFriendInfo;
};
