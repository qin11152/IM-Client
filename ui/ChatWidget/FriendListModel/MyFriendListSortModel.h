#pragma once

#include "module/stdafx.h"
#include <QSortFilterProxyModel>

class MyFriendListSortModel  : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	MyFriendListSortModel(QObject* parent = nullptr);
	~MyFriendListSortModel();

	void setFilterMap(const std::map<int, QVariant>& mapFilter);

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex& source_parent)const override;

private:
	std::map<int, QVariant> m_mapFilter;
};
