#include "ui/LogInWidget/LogInWidget.h"
#include  "module/Log/Log.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	qRegisterMetaType<std::string>("std::string");
	//跨平台获取当前程序所在路径
	QString strPath = QCoreApplication::applicationDirPath();
	//设置log4cxx配置文件路径
	std::string strLog4cxxPath = strPath.toStdString() + "/log4cxx.properties";
	Logcxx::get_mutable_instance().Init(strLog4cxxPath.c_str());
	_LOG(Logcxx::Level::INFO, "main start");
	LogInWidget w;
	w.show();
	return a.exec();
}
