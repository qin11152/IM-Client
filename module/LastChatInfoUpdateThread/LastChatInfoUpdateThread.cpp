#include "LastChatInfoUpdateThread.h"
#include "module/DataBaseDelegate/DataBaseDelegate.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"

LastChatInfoUpdateThread::LastChatInfoUpdateThread(QObject *parent)
    : QThread(parent)
{}

LastChatInfoUpdateThread::~LastChatInfoUpdateThread()
{}

void LastChatInfoUpdateThread::run()
{
    //�ȱ��ݣ���ֹ�޸ĵ�ʱ�����رգ����ݶ�ʧ
    DataBaseDelegate::Instance()->copyLastChatToBackUps();
    //Ȼ�����lastchat
    DataBaseDelegate::Instance()->deleteLastChatInfo();
    //Ȼ���ȡ��ǰ��˳��
    QStringList newModelOrder;
    ChatWidgetManager::Instance()->onSignalGetModelOrder(newModelOrder);
    //�ѵ�˳��������
    for (int i = 0; i < newModelOrder.size(); ++i)
    {
        DataBaseDelegate::Instance()->insertLastChat(newModelOrder[i], QString::number(i));
    }
}
