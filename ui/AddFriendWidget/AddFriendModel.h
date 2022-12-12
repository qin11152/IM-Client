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
    void updateModel(const QString& id, bool validState);

    void insertRow(const std::vector<AddFriendInfo>& addFriendInfo);

protected:
    QVariant data(QModelIndex const& index, int role) const override;
    int rowCount(QModelIndex const& parent = QModelIndex()) const override;

private:
    std::vector<AddFriendInfo> m_vecAddFriendInfo;
};
