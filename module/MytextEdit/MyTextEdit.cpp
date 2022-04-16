#include "MyTextEdit.h"

MyTextEdit::MyTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
}

MyTextEdit::~MyTextEdit()
{
}

void MyTextEdit::focusInEvent(QFocusEvent* e)
{
    emit signalTextEditIsFocus(true);
}

void MyTextEdit::focusOutEvent(QFocusEvent* e)
{
    emit signalTextEditIsFocus(false);
}
