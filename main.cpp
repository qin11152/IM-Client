#include "ui/LogInWidget/LogInWidget.h"
#include "module/Log/Log.h"
#include <QtWidgets/QApplication>

#include "module/ThreadPool/ThreadPool.h"
#include "module/TCPThread/TCPOperateInterface.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ThreadPool::get_mutable_instance().startPool();
	module::TCPOperateInterface::get_mutable_instance();
	//打印threadpool地址
	qRegisterMetaType<std::string>("std::string");
	//跨平台获取当前程序所在路径
	QString strPath = QCoreApplication::applicationDirPath();
	//设置log4cxx配置文件路径
	std::string strLog4cxxPath = strPath.toStdString() + "/log4cxx.properties";
	Logcxx::get_mutable_instance().Init(strLog4cxxPath.c_str());
	_LOG(Logcxx::Level::INFO, "main start");

	wechat::LogInWidget w;
	w.show();
	return a.exec();
}
