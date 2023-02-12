#include "ui/LogInWidget/LogInWidget.h"
#include  "module/Log/Log.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	qRegisterMetaType<std::string>("std::string");
	//��ƽ̨��ȡ��ǰ��������·��
	QString strPath = QCoreApplication::applicationDirPath();
	//����log4cxx�����ļ�·��
	std::string strLog4cxxPath = strPath.toStdString() + "/log4cxx.properties";
	Logcxx::get_mutable_instance().Init(strLog4cxxPath.c_str());
	_LOG(Logcxx::Level::INFO, "main start");
	LogInWidget w;
	w.show();
	return a.exec();
}
