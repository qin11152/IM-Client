#include "MyLineEdit.h"
#include "../../module/MyDefine.h"

MyLineEdit::MyLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    ui.setupUi(this);
}

MyLineEdit::~MyLineEdit()
{
}

void MyLineEdit::focusInEvent(QFocusEvent* e)
{
    emit signalSwitchStackedWidget(SearchWidget);
    QLineEdit::focusInEvent(e);
}

void MyLineEdit::focusOutEvent(QFocusEvent* e)
{
    emit signalSwitchStackedWidget(FriendListWidget);
    QLineEdit::focusOutEvent(e);
}

void MyLineEdit::dragLeaveEvent(QDragLeaveEvent* e)
{
    QWidget::dragLeaveEvent(e);
}
