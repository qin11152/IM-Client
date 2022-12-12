#pragma once

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
     * brief���ػ��¼��������Լ���Ҫ������.
     * 
     * \param painter������
     * \param option������
     * \param index��λ��
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)const override;

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

    /**
     * brief�����سߴ�.
     * 
     * \param option
     * \param index
     * \return 
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index)const override;
};
