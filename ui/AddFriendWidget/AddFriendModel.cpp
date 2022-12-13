#include "AddFriendModel.h"

AddFriendModel::AddFriendModel(QObject *parent)
    : QAbstractListModel(parent)
{}

AddFriendModel::~AddFriendModel()
{}

void AddFriendModel::setData(std::vector<AddFriendInfo>&addFriendInfo)
{
    beginResetModel();
    m_vecAddFriendInfo = addFriendInfo;
    endResetModel();
}

//根据id修改模型中的状态
void AddFriendModel::updateModelValidState(const QString& id, bool validState)
{
    for (auto& item : m_vecAddFriendInfo)
    {
        if (item.m_strId == id)
        {
            item.isValid = validState;
            break;
        }
    }
}

void AddFriendModel::updateModelImagePath(const QString& id, const QString& path)
{
    for (auto& item : m_vecAddFriendInfo)
    {
        if (item.m_strId == id)
        {
            item.m_strProfileImagePath = path;
            break;
        }
    }
}

void AddFriendModel::insertRow(const std::vector<AddFriendInfo>& addFriendInfo)
{
    int cnt = addFriendInfo.size();
    beginInsertRows(QModelIndex(), 0, cnt - 1);
    m_vecAddFriendInfo.insert(m_vecAddFriendInfo.begin(), addFriendInfo.begin(), addFriendInfo.end());
    endInsertRows();
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
        return m_vecAddFriendInfo[index.row()].m_strId;
        break;
    case (int)UserRoleDefine::AddFriendName:
        return m_vecAddFriendInfo[index.row()].m_strFriendName;
        break;
    case (int)UserRoleDefine::AddFriendImagePath:
        return m_vecAddFriendInfo[index.row()].m_strProfileImagePath;
        break;
    case (int)UserRoleDefine::AddFriednVerifyInfo:
        return m_vecAddFriendInfo[index.row()].m_strVerifyInfo;
        break;
    case (int)UserRoleDefine::AddFriendValid:
        return m_vecAddFriendInfo[index.row()].isValid;
        break;
    default:
        return QVariant();
    }

    return QVariant();
}

int AddFriendModel::rowCount(QModelIndex const& parent) const
{
    return m_vecAddFriendInfo.size();
}
