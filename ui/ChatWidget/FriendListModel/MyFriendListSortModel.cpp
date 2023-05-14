#include "MyFriendListSortModel.h"
#include "module/PublicFunction/PublicFunction.h"

MyFriendListSortModel::MyFriendListSortModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{}

MyFriendListSortModel::~MyFriendListSortModel()
{}

void MyFriendListSortModel::setFilterMap(const std::map<int, QVariant>&mapFilter)
{
	m_mapFilter = mapFilter;
	invalidateFilter();
}

bool MyFriendListSortModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	if (0 < m_mapFilter.size())
	{
		bool bMatch = false;
		//根据行、列和父对象获取模型中的数据
		//因为在筛选行，所以列为0
		QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
		for (auto& filterItem : m_mapFilter)
		{
			QVariant rule = filterItem.second;
			QVariant mode = sourceIndex.data(filterItem.first);
			//搜索框输入的名字
			auto filterStr = filterItem.second.toString();
			//模型中这个人的名字
			auto nameOfModel = sourceIndex.data(filterItem.first).toString();
			nameOfModel = Base::PinYin::convertToPinYin(nameOfModel);
			//qDebug() << "filter:" << filterStr << "name:" << nameOfModel;
			//如果包含搜索的这个内容就认为搜索成功
			if (nameOfModel.contains(filterStr))
			{
				bMatch = true;
				break;
			}
		}
		return bMatch;
	}
	return true;
}
