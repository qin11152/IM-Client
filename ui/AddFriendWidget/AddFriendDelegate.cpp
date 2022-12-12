#include "MyDefine.h"
#include "AddFriendDelegate.h"

#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

AddFriendDelegate::AddFriendDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

AddFriendDelegate::~AddFriendDelegate()
{}

void AddFriendDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    //QStyledItemDelegate::paint(painter, option, index);

    QString strId = index.data((int)UserRoleDefine::AddFriendName).toString();
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

    if (!isValid)
    {
        QStyleOptionButton button;
        button.rect = QRect(option.rect.right() - 120, option.rect.y() + 10, 80, 40);
        //button.state |= QStyle::State_Enabled;
        button.text = QString::fromLocal8Bit("同意");

        QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
    }
    else
    {
        painter->drawText(QRect{ option.rect.right() - 120, option.rect.y() + 10, 80, 40 },Qt::AlignVCenter|Qt::AlignHCenter, QString::fromLocal8Bit("已添加"));
    }
}

bool AddFriendDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    auto pEvent = reinterpret_cast<QMouseEvent*>(event);
    if (pEvent->button() == Qt::LeftButton)
    {
        if (option.rect.contains(pEvent->pos()))
        {
            QString id = index.data(int(UserRoleDefine::AddFriendId)).toString();
            emit signalAgreeAdd(id);
            return true;
        }
    }
    return false;
}

QSize AddFriendDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize({ 800,60 });
}
