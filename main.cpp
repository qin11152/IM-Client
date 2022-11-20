#include "ui/LogInWidget/LogInWidget.h"
#include  "module/Log/Log.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::string>("std::string");
    Logcxx::get_mutable_instance().Init("./log4cxx.properties");
    LogInWidget w;
    w.show();
    return a.exec();
}
