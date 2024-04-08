#pragma once

#include <QTextEdit>


namespace wechat
{
    class MyTextEdit : public QTextEdit
    {
        Q_OBJECT

    public:
        MyTextEdit(QWidget* parent);
        ~MyTextEdit();
    signals:
        void signalTextEditIsFocus(bool isFocus);
    protected:
        void focusInEvent(QFocusEvent* e)override;
        void focusOutEvent(QFocusEvent* e)override;
    };
}
