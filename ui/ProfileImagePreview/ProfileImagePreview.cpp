#include "ProfileImagePreview.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "module/TCPThread/TCPThread.h"
#include <QImageReader>
#include <QPainter>
#include <QFileDialog>
#include <QBuffer>
#include <fstream>

ProfileImagePreview::ProfileImagePreview(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    //隐藏最大化按钮
    setWindowFlag(Qt::WindowMaximizeButtonHint, true);
    setFixedSize(QSize(400,400));
    setWindowTitle("");

    initConnect();
}

ProfileImagePreview::~ProfileImagePreview()
{}

void ProfileImagePreview::setImagePath(const QString& path, const int id)
{
    m_strPagePath = path;
    m_iPageId = id;
}

void ProfileImagePreview::onSignalChooseBtnClicked()
{
    //用户点击选择头像按钮
    QFileDialog* fileDialog = new QFileDialog(this);

    //定义文件对话框标题
    fileDialog->setWindowTitle(QString::fromLocal8Bit("选择图片"));

    //设置打开的文件路径
    fileDialog->setDirectory("./");

    //设置文件过滤器,只显示jpg,png, 文件,多个过滤文件使用空格隔开
    fileDialog->setNameFilter(tr("File(*.jpg* *.png* *.jpeg*)"));

    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);

    fileDialog->show();

    //获取选择的文件的路径
    QStringList fileNames;
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }

    if (fileNames.size() > 0)
    {
        m_strPagePath = fileNames[0];
        //TODO 发送给服务器，修改本地存储的
        QImage image(m_strPagePath);
        image = image.scaled(100, 100);
        QByteArray byteArr;
        QBuffer buffer(&byteArr);
        QFileInfo info(m_strPagePath);
        QString suffix = info.suffix();//获取文件后缀
        QByteArray stc = suffix.toLatin1();
        image.save(&buffer, stc.data());//将QString类型的后缀名改为char*
        QString str_base64 = byteArr.toBase64();
        QString name = "suibian";
        TCPThread::get_mutable_instance().sendImageMsg(str_base64, name, stc.data());
        //emit signalSendImageMsg(str_base64, name, stc.data());
        QString curPath=QApplication::applicationDirPath();
        curPath += "/data/image/my." + stc;
        image.save(curPath);
        update();
    }
}

void ProfileImagePreview::onSignalUpdateProfileImage()
{
}

void ProfileImagePreview::paintEvent(QPaintEvent* event)
{
    if(m_strPagePath.isEmpty())
    {
        m_strPagePath = kDefaultProfileImageWidget;
    }
    QRect rect = ui.widget->rect();
    QPainter p(this);
    QImageReader reader;
    QImage image;
    reader.setDecideFormatFromContent(true);
    reader.setScaledSize(QSize(rect.width(),rect.height()));
    reader.setFileName(m_strPagePath);
    if (reader.canRead())
    {
        if (!reader.read(&image))
        {
            QImageReader::ImageReaderError error = reader.error();
            QString strError = reader.errorString();
            printf("last error:%s\n", strError.toStdString().c_str());
            return;
        }
    }
    p.drawPixmap(40,50,330,330, QPixmap::fromImage(image)/*.scaled(width(), height(), Qt::KeepAspectRatio)*/);
}

void ProfileImagePreview::showEvent(QShowEvent* event)
{
    if(m_iPageId!=PublicDataManager::get_mutable_instance().getMyId().toInt())
    {
        ui.changeProfileImagePushButton->hide();
    }
    else
    {
        ui.changeProfileImagePushButton->show();
    }
}

void ProfileImagePreview::initConnect()
{
    connect(ui.changeProfileImagePushButton, &QPushButton::clicked, this, &ProfileImagePreview::onSignalChooseBtnClicked);
    connect(this, &ProfileImagePreview::signalSendImageMsg, &TCPThread::get_mutable_instance(), &TCPThread::sendImageMsg, Qt::QueuedConnection);
}
