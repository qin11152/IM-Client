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
     * brief������id�޸�model�е�״̬.
     * 
     * \param id��id
     * \param validState����֤״̬
     */
    void updateModel(const QString& id, bool validState);

    void insertRow(const std::vector<AddFriendInfo>& addFriendInfo);

protected:
    QVariant data(QModelIndex const& index, int role) const override;
    int rowCount(QModelIndex const& parent = QModelIndex()) const override;

private:
    std::vector<AddFriendInfo> m_vecAddFriendInfo;
};
