#include "MyDefine.h"
#include "AddFriendDelegate.h"
#include <QPainter>
#include <QApplication>

AddFriendDelegate::AddFriendDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

AddFriendDelegate::~AddFriendDelegate()
{}

void AddFriendDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    //QStyledItemDelegate::paint(painter, option, index);

    QString strId = index.data((int)UserRoleDefine::AddFriendId).toString();
    QString strImagePath = index.data((int)UserRoleDefine::AddFriendImagePath).toString();
    QString strVerifyInfo = index.data((int)UserRoleDefine::AddFriednVerifyInfo).toString();
    bool isValid = index.data((int)UserRoleDefine::AddFriendValid).toBool();

    QRect imageRect(option.rect.x() + 20, option.rect.y() + 10, 40, 40);
    QRect idRect(option.rect.x() + 80, option.rect.y() + 10, 100, 20);
    QRect verifyRect(option.rect.x() + 80, option.rect.y() + 30, 300, 30);

    auto displayOption = option;
    displayOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;

    painter->drawPixmap(imageRect, QPixmap(strImagePath));
    painter->drawText(idRect, strId);
    painter->drawText(verifyRect, strVerifyInfo);

    QStyleOptionButton button;
    button.rect = QRect(option.rect.right() - 230, option.rect.y() - 10, 200, 40);
    button.state |= QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool AddFriendDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    return true;
}

QSize AddFriendDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize({ 800,60 });
}
