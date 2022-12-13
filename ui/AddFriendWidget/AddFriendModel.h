#pragma once

#include "MyDefine.h"

#include <QAbstractListModel>

class AddFriendModel  : public QAbstractListModel
{
    Q_OBJECT

public:
    AddFriendModel(QObject* parent = nullptr);
    ~AddFriendModel();

    void setData(std::vector< AddFriendInfo>& addFriendInfo);
    
    /**
     * brief：根据id修改model中的状态.
     * 
     * \param id：id
     * \param validState：验证状态
     */
    void updateModelValidState(const QString& id, bool validState);

    /**
     * brief：更新模型中的头像路径.
     * 
     * \param id：id
     * \param path：头像路径
     */
    void updateModelImagePath(const QString& id, const QString& path);

    void insertRow(const std::vector<AddFriendInfo>& addFriendInfo);

protected:
    QVariant data(QModelIndex const& index, int role) const override;
    int rowCount(QModelIndex const& parent = QModelIndex()) const override;

private:
    std::vector<AddFriendInfo> m_vecAddFriendInfo;
};
