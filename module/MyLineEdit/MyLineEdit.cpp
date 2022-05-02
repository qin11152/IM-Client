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
    emit signalIsFocus(true);
    QLineEdit::focusInEvent(e);
}

void MyLineEdit::focusOutEvent(QFocusEvent* e)
{
    emit signalIsFocus(false);
    QLineEdit::focusOutEvent(e);
}

void MyLineEdit::dragLeaveEvent(QDragLeaveEvent* e)
{
    QWidget::dragLeaveEvent(e);
}
