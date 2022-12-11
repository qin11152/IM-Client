#include "AddFriendModel.h"

AddFriendModel::AddFriendModel(QObject *parent)
    : QStandardItemModel(parent)
{}

AddFriendModel::~AddFriendModel()
{}

void AddFriendModel::setData(std::vector<AddFriendInfo>&addFriendInfo)
{
    beginResetModel();
    m_vecAddFriendInfo = addFriendInfo;
    endResetModel();
}

QVariant AddFriendModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_vecAddFriendInfo.size())
    {
        return QVariant();
    }
    switch (role)
    {
    case (int)UserRoleDefine::AddFriendId:
        return m_vecAddFriendInfo[index.row()].m_strFriendId;
    case (int)UserRoleDefine::AddFriendImagePath:
        return m_vecAddFriendInfo[index.row()].m_strProfileImagePath;
    case (int)UserRoleDefine::AddFriednVerifyInfo:
        return m_vecAddFriendInfo[index.row()].m_strVerifyInfo;
    case (int)UserRoleDefine::AddFriendValid:
        return m_vecAddFriendInfo[index.row()].isValid;
    default:
        return QVariant();
    }

    return QVariant();
}

int AddFriendModel::rowCount(QModelIndex const& parent) const
{
    return m_vecAddFriendInfo.size();
}
