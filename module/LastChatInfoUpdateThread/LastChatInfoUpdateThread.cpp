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
    //先备份，防止修改的时候程序关闭，内容丢失
    DataBaseDelegate::Instance()->copyLastChatToBackUps();
    //然后清空lastchat
    DataBaseDelegate::Instance()->deleteLastChatInfo();
    //然后获取当前的顺序
    QStringList newModelOrder;
    ChatWidgetManager::Instance()->onSignalGetModelOrder(newModelOrder);
    //把的顺序存入表中
    for (int i = 0; i < newModelOrder.size(); ++i)
    {
        DataBaseDelegate::Instance()->insertLastChat(newModelOrder[i], QString::number(i));
    }
}
