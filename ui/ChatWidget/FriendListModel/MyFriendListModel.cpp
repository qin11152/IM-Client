#include "MyFriendListModel.h"

MyFriendListModel::MyFriendListModel(QObject *parent)
	: QAbstractListModel(parent)
{}

MyFriendListModel::~MyFriendListModel()
{}

void MyFriendListModel::setData(std::vector<MyFriendInfoWithFirstC>& friendListInfo)
{
	beginResetModel();
	m_vecFriendListInfo = friendListInfo;
	endResetModel();
}

void MyFriendListModel::addData(const MyFriendInfoWithFirstC& friendListInfo)
{
	beginInsertRows(QModelIndex(), 0, 0);
	m_vecFriendListInfo.insert(m_vecFriendListInfo.begin(), friendListInfo);
	endInsertRows();
}

QVariant MyFriendListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}
	switch (role)
	{
	case (int)UserRoleDefine::FriendListId:
		return m_vecFriendListInfo[index.row()].m_strId.c_str();
	case (int)UserRoleDefine::FriendListName:
		return m_vecFriendListInfo[index.row()].m_strName.c_str();
	case (int)UserRoleDefine::FriendListImagePath:
		return m_vecFriendListInfo[index.row()].m_strImagePath.c_str();
	default:
		return QVariant();
	}		
	return QVariant();
}

int MyFriendListModel::rowCount(QModelIndex const& parent) const
{
	return m_vecFriendListInfo.size();
}

QMap<int, QVariant> MyFriendListModel::itemData(const QModelIndex& index) const
{
	QMap<int, QVariant> roles;

	int iDataIndex = index.row();
	
	auto tmpInfo = m_vecFriendListInfo[iDataIndex];
	
	roles.insert((int)UserRoleDefine::FriendListId, tmpInfo.m_strId.c_str());
	roles.insert((int)UserRoleDefine::FriendListName, tmpInfo.m_strName.c_str());
	roles.insert((int)UserRoleDefine::FriendListImagePath, tmpInfo.m_strImagePath.c_str());
	
	return roles;
}

QHash<int, QByteArray> MyFriendListModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[(int)UserRoleDefine::FriendListName] = "name";
	roles[(int)UserRoleDefine::FriendListImagePath] = "imagePath";
	roles[(int)UserRoleDefine::FriendListId] = "idx";
	return roles;
}
