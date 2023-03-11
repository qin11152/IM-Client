#pragma once

#include "module/stdafx.h"
#include <QSortFilterProxyModel>

class ChooseFriendSortModel  : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	ChooseFriendSortModel(QObject* parent = nullptr);
	~ChooseFriendSortModel();

	void setFilterMap(const std::map<int, QVariant>& mapFilter);

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex& source_parent)const override;

private:
	std::map<int, QVariant> m_mapFilter;
};
