﻿#pragma once

#include <QStyledItemDelegate>

class AddFriendDelegate  : public QStyledItemDelegate
{
    Q_OBJECT

public:
    AddFriendDelegate(QObject* parent = nullptr);
    ~AddFriendDelegate();

signals:
    void signalAgreeAdd(const QString& id);

protected:
    /**
     * brief：重绘事件，绘制自己需要的内容.
     * 
     * \param painter：画笔
     * \param option：区域
     * \param index：位置
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)const override;

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

    /**
     * brief：返回尺寸.
     * 
     * \param option
     * \param index
     * \return 
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index)const override;
};
