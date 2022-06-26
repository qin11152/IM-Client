#pragma once

#include <QThread>

class LastChatInfoUpdateThread  : public QThread
{
    Q_OBJECT

public:
    LastChatInfoUpdateThread(QObject *parent);
    ~LastChatInfoUpdateThread();
    void setLastChatQML(QObject* ptr) { m_ptrLastChatQML = ptr; }

protected:
    void run()override;

private:
    QObject* m_ptrLastChatQML{ nullptr };
};
