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
		//�����С��к͸������ȡģ���е�����
		//��Ϊ��ɸѡ�У�������Ϊ0
		QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
		for (auto& filterItem : m_mapFilter)
		{
			QVariant rule = filterItem.second;
			QVariant mode = sourceIndex.data(filterItem.first);
			//���������������
			auto filterStr = filterItem.second.toString();
			//ģ��������˵�����
			auto nameOfModel = sourceIndex.data(filterItem.first).toString();
			nameOfModel = Base::PinYin::convertToPinYin(nameOfModel);
			//qDebug() << "filter:" << filterStr << "name:" << nameOfModel;
			//�������������������ݾ���Ϊ�����ɹ�
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
