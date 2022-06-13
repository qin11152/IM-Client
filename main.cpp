#include "ui/LogInWidget/LogInWidget.h"
#include "ui/ChatWidget/ChatWidget.h"
#include "module/PublicFunction/PublicFunction.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LogInWidget w;
    w.show();
    /*ChatWidget* w = new ChatWidget("1");
    w->show();*/
    return a.exec();
}
