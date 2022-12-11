#pragma once

#include "MyDefine.h"

#include <QStandardItemModel>

class AddFriendModel  : public QStandardItemModel
{
    Q_OBJECT

public:
    AddFriendModel(QObject* parent = nullptr);
    ~AddFriendModel();

    void setData(std::vector< AddFriendInfo>& addFriendInfo);

protected:
    QVariant data(QModelIndex const& index, int role) const override;
    int rowCount(QModelIndex const& parent = QModelIndex()) const override;

private:
    std::vector<AddFriendInfo> m_vecAddFriendInfo;
};
