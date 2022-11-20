#include "ui/LogInWidget/LogInWidget.h"
#include  "module/Log/Log.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::string>("std::string");
    Logcxx::get_mutable_instance().Init("./log4cxx.properties");
    //_LOG(Logcxx::Level::INFO, "test");
    LogInWidget w;
    w.show();
    /*ChatWidget* w = new ChatWidget("1");
    w->show();*/
    return a.exec();
}
