#include "ChooseFriendModel.h"

ChooseFriendModel::ChooseFriendModel(QObject *parent)
	: QAbstractListModel(parent)
{}

ChooseFriendModel::~ChooseFriendModel()
{}

void ChooseFriendModel::setData(std::vector<MyFriendInfoWithFirstC>&friendInfo)
{
	beginResetModel();
	m_vecFriendInfo = friendInfo;
	endResetModel();
}

QVariant ChooseFriendModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= m_vecFriendInfo.size())
	{
		return QVariant();
	}
	switch (role)
	{
	case (int)UserRoleDefine::FriendListId:
		return m_vecFriendInfo[index.row()].m_strId.c_str();
		break;
	case (int)UserRoleDefine::FriendListName:
		return m_vecFriendInfo[index.row()].m_strName.c_str();
		break;
	case (int)UserRoleDefine::FriendListImagePath:
		return m_vecFriendInfo[index.row()].m_strImagePath.c_str();
		break;
	default:
		return QVariant();
	}

	return QVariant();
}

int ChooseFriendModel::rowCount(QModelIndex const& parent) const
{
	return m_vecFriendInfo.size();
}

QMap<int, QVariant> ChooseFriendModel::itemData(const QModelIndex& index) const
{
	QMap<int, QVariant> roles;

	int iDataIndex = index.row();

	auto tmpInfo = m_vecFriendInfo[iDataIndex];

	roles.insert((int)UserRoleDefine::FriendListId, tmpInfo.m_strId.c_str());
	roles.insert((int)UserRoleDefine::FriendListName, tmpInfo.m_strName.c_str());
	roles.insert((int)UserRoleDefine::FriendListImagePath, tmpInfo.m_strImagePath.c_str());

	return roles;
}

QHash<int, QByteArray> ChooseFriendModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[(int)UserRoleDefine::FriendListName] = "name";
	roles[(int)UserRoleDefine::FriendListImagePath] = "imagePath";
	roles[(int)UserRoleDefine::FriendListId] = "idx";
	return roles;
}
