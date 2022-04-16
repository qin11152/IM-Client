#include "SQLDelegate.h"
#include <QApplication>

using SingletonPtr = std::shared_ptr<SQLDelegate>;
//��ʼ����̬��Ա����
SingletonPtr SQLDelegate::m_SingletonPtr = nullptr;
std::mutex SQLDelegate::m_mutex;

SQLDelegate::SQLDelegate(QObject *parent=nullptr)
    : QObject(parent)
{
}

void SQLDelegate::init()
{
    //�ڹ��캯���к����ݿ��������
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //��������ڱ�Ŀ¼�����ɣ����û�оͻὨ��������ݿ�
    db.setDatabaseName(QApplication::applicationDirPath() + "/Record.dat");
    if(!db.open())
    {
        //TODO��־�������¼����
    }
}

SQLDelegate::~SQLDelegate()
{
}

SingletonPtr SQLDelegate::Instance()
{
    //˫�ر�������һ���пգ��������ɶ��
    if (m_SingletonPtr == nullptr)
    {
        std::lock_guard<std::mutex> lck(m_mutex);
        //�������пգ�������߳����ɶ��ʵ��
        if (m_SingletonPtr == nullptr)
        {
            m_SingletonPtr = std::shared_ptr<SQLDelegate>(new SQLDelegate);
        }
    }
    //����ָ��
    return m_SingletonPtr;
}
