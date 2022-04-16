#include "ui/LogInWidget/LogInWidget.h"
#include "ui/ChatWidget/ChatWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*LogInWidget w;
    w.show();*/
    ChatWidget w(1);
    w.show();
    return a.exec();
}
