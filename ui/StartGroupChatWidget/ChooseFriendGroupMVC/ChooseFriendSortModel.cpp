#include "ChooseFriendSortModel.h"

ChooseFriendSortMOdel::ChooseFriendSortMOdel(QObject *parent)
	: QSortFilterProxyModel(parent)
{}

ChooseFriendSortMOdel::~ChooseFriendSortMOdel()
{}

void ChooseFriendSortMOdel::setFilterMap(const std::map<int, QVariant>&mapFilter)
{
}

bool ChooseFriendSortMOdel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	return false;
}
