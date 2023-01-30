#include "MyFriendListModel.h"

MyFriendListModel::MyFriendListModel(QObject *parent)
	: QAbstractListModel(parent)
{}

MyFriendListModel::~MyFriendListModel()
{}

void MyFriendListModel::setData(std::vector<FriendListInfo>& friendListInfo)
{
	beginResetModel();
	m_vecFriendListInfo = friendListInfo;
	endResetModel();
}

void MyFriendListModel::addData(const FriendListInfo& friendListInfo)
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
		return m_vecFriendListInfo[index.row()].m_strFriendId;
	case (int)UserRoleDefine::FriendListName:
		return m_vecFriendListInfo[index.row()].m_strFriendName;
	case (int)UserRoleDefine::FriendListImagePath:
		return m_vecFriendListInfo[index.row()].m_strFriendImagePath;
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
	
	roles.insert((int)UserRoleDefine::FriendListId, tmpInfo.m_strFriendId);
	roles.insert((int)UserRoleDefine::FriendListName, tmpInfo.m_strFriendName);
	roles.insert((int)UserRoleDefine::FriendListImagePath, tmpInfo.m_strFriendImagePath);
	
	return roles;
}

QHash<int, QByteArray> MyFriendListModel::roleNames() const
{
	return QHash<int, QByteArray>();
}
