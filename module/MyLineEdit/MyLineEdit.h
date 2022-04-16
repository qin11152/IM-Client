#pragma once

#include <QLineEdit>
#include "ui_MyLineEdit.h"

class MyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    MyLineEdit(QWidget *parent = Q_NULLPTR);
    ~MyLineEdit();
    
signals:
    void signalSwitchStackedWidget(int type);

protected:
    void focusInEvent(QFocusEvent* e)override;
    void focusOutEvent(QFocusEvent* e)override;
    void dragLeaveEvent(QDragLeaveEvent* e)override;

private:
    Ui::MyLineEdit ui;
};
