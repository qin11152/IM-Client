#include "NotifyLabel.h"
#include <QDesktopWidget>
#include <QApplication>

constexpr int kDefaultWidth = 800;
constexpr int kDefaultHeight = 100;

NotifyLabel::NotifyLabel(QWidget *parent)
    : QLabel(parent)
{
    
    if (!parent)
    {
        QDesktopWidget* desktopWidget = QApplication::desktop();
        QRect screenRect = desktopWidget->screenGeometry();
        int screenWidth = screenRect.width();
        int screenHeight = screenRect.height();
        m_rect = QRect((screenWidth - kDefaultWidth) / 2, screenHeight - kDefaultHeight, kDefaultWidth, kDefaultHeight);
    }
    else
    {
        int width = parent->width() * 0.5;
        int height = width / 8;
        m_rect = QRect((parent->width() - width) / 2, parent->height() - height, width, height);
    }
    setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_timer = new QTimer(this);
    m_scrollTimer = new QTimer(this);
    m_scrollTimer->setInterval(400);
    connect(m_scrollTimer, &QTimer::timeout, this, &NotifyLabel::onSignalScrollTimeout);
    connect(m_timer, &QTimer::timeout, this, &NotifyLabel::onSignalTimeout);
}

NotifyLabel::~NotifyLabel()
{}

void NotifyLabel::setPositions(int x, int y, int w, int h)
{
    m_rect = QRect(x, y, w, h);
}

void NotifyLabel::setNitifyText(const char* notifyInfo)
{
    setText(notifyInfo);
    m_strNotifyInfo = notifyInfo;
}

void NotifyLabel::showNotify()
{
    if (!m_timer)
    {
        m_timer = new QTimer(this);
    }
    setGeometry(m_rect);
    //qt���ݸ߶����������С
    QFont font = this->font();
    font.setPixelSize(m_rect.height() * 0.4);
    setFont(font);
    //qt����label��ʽ��Բ�Ǿ��Σ������ɫ���������
    setStyleSheet(QString("QLabel{border-radius:%1px;background-color:rgb(119, 221, 255);color:rgb(255, 0, 0);}").arg(m_rect.height() / 2));

    upateLabelRollingState();
    m_timer->setInterval(m_iTimerInterval);
    m_timer->start();
    m_bIsRunning = true;
    show();
}

void NotifyLabel::stopNotify()
{
    if (m_timer && m_timer->isActive())
    {
        m_timer->stop();
    }
    if (m_scrollTimer && m_scrollTimer->isActive())
    {
        m_scrollTimer->stop();
    }
    hide();
    m_iTimerInterval = 5 * 1000;
    m_iScrollDisplayPos = 0;
    m_bIsRunning = false;
}

void NotifyLabel::setShowTime(const int interval)
{
    m_iTimerInterval = interval;
}

//�ж�label�е��ı��Ƿ���Ҫ�������ʾ
void NotifyLabel::upateLabelRollingState()
{
    QFontMetrics fm(font());
    int textWidth = fm.width(text());
    if (textWidth > m_rect.width())
    {
        m_scrollTimer->start();
        setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
    else
    {
        setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
}

void NotifyLabel::onSignalScrollTimeout()
{
    m_scrollTimer->stop();
    if (m_iScrollDisplayPos > m_strNotifyInfo.length()) 
    {
        m_iScrollDisplayPos = 0;
    }
    setText(m_strNotifyInfo.mid(m_iScrollDisplayPos));
    m_iScrollDisplayPos++;
    m_scrollTimer->start();
}

void NotifyLabel::onSignalTimeout()
{
    m_timer->stop();
    m_iTimerInterval = 5 * 1000;
    m_iScrollDisplayPos = 0;
    hide();
    m_bIsRunning = false;
    if (m_scrollTimer&&m_scrollTimer->isActive())
    {
        m_scrollTimer->stop();
    }
}
