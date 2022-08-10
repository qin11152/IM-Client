#include "RegisterWidget.h"
#include "ui_RegisterWidget.h"
#include "../../module/TCPConnect/TCPConnect.h"
#include "../../protocol/RegisterJsonData/RegisterJsonData.h"
#include "../../protocol/RegisterReplyData/RegisterReplyData.h"
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QMessageBox>
#include <QCloseEvent>


RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget(parent)
{
    ui = new Ui::RegisterWidget();
    ui->setupUi(this);
    initUi();
    initConnect();
}

void RegisterWidget::cleanLineEdit()
{
    ui->firstPasswordLineEdit->clear();
    ui->secondPasswordLineEdit->clear();
    ui->nickLineEdit->clear();
}

void RegisterWidget::closeEvent(QCloseEvent* event)
{
    emit signalShowLoginInWidget();
    event->accept();
}

void RegisterWidget::onNickLineEditEditingFinished(const QString& text)
{
}

void RegisterWidget::onfirstPasswordLineEditEditingFinished(const QString&text)
{
    QRegularExpression rx("^(?![0-9]+$)(?![a-zA-Z]+$)[0-9A-Za-z]{8,}$");
    //QRegExp rx("^(?!(?:[^a-zA-Z]+|\D+|[a-zA-Z0-9]+)$).{8,}$");
    //�õ��ȶԽ��
    auto machResult = rx.match(text);
    if(!machResult.hasMatch())
    {
        ui->label_2->setVisible(true);
    }
    else
    {
        ui->label_2->setVisible(false);
    }

    if (!ui->secondPasswordLineEdit->text().isEmpty())
    {
        if (ui->secondPasswordLineEdit->text() != text)
        {
            ui->label->setVisible(true);
        }
        else
        {
            ui->label->setVisible(false);
        }
    }

    if (text.isEmpty())
    {
        ui->label_2->setVisible(false);
    }
}

void RegisterWidget::onsecondPasswordLineEditEditingFinished(const QString& text)
{
    if (text != ui->firstPasswordLineEdit->text())
    {
        ui->label->setVisible(true);
    }
    else
    {
        ui->label->setVisible(false);
    }
    if (text.isEmpty() && ui->firstPasswordLineEdit->text().isEmpty())
    {
        ui->label->setVisible(false);
    }
}

void RegisterWidget::onRegisterClicked()
{
    //qDebug() << "clicked";
    if (ui->firstPasswordLineEdit->text().isEmpty() || ui->secondPasswordLineEdit->text().isEmpty()
        || ui->nickLineEdit->text().isEmpty() || ui->label->isVisible() || ui->label_2->isVisible())
    {
        return;
    }
    else
    {
        //qDebug() << "edit content is:" << ui->nickLineEdit->text() << "  " << ui->firstPasswordLineEdit->text();
        RegisterJsonData reg("");
        reg.m_strUserName = ui->nickLineEdit->text().toStdString();
        reg.m_strUserPassword = ui->firstPasswordLineEdit->text().toStdString();
        reg.m_strType = MessageType::RegisterRequest;
        //qDebug() << "reg content is:" << reg.m_strUserName.c_str() << "  " << reg.m_strUserPassword.c_str();
        std::string registerMessage = reg.generateJson();
        TCPConnect::Instance()->sendMessage(registerMessage);
        return;
    }
}

void RegisterWidget::onMsgHandle(const QString& msg)
{
    //printf(msg.toStdString().c_str());
    ptree m_ptree;
    std::stringstream sstream(msg.toStdString().c_str());
    read_json(sstream, m_ptree);
    int messageType = m_ptree.get<int>("type");
    qDebug() << messageType;
    if (static_cast<MessageTypeBaseType>(MessageType::RegisterResponse) == messageType)
    {
        const bool registerResult = m_ptree.get<bool>("RegisterResult");
        const int id = m_ptree.get<int>("UserId");
        if (registerResult == true)
        {
            const QString tmp = QString::fromLocal8Bit("ע��ɹ�,����idΪ") + QString::number(id) + QString::fromLocal8Bit("���μ�");
            switch (QMessageBox::information(this, QString::fromLocal8Bit("ע����"),
                tmp, QMessageBox::Ok),0)
            {
            case 0:
            {
                hide();
                emit signalShowLoginInWidget();
                break;
            }
            default:
                break;
            }
        }
        else
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("ע��ʧ��"),
                QString::fromLocal8Bit("δ�ܳɹ�ע�ᣬ���ٴγ���"),
                QMessageBox::Ok);
        }
    }
}

void RegisterWidget::onSignalConnectedFailed()
{
    QMessageBox::warning(this, QString::fromLocal8Bit("�޷����ӷ�����"),
        QString::fromLocal8Bit("δ����������������ӣ���������������"),
        QMessageBox::Ok);
}

void RegisterWidget::initConnect()
{
    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterWidget::onRegisterClicked);
    connect(ui->nickLineEdit, &QLineEdit::textChanged, this, &RegisterWidget::onNickLineEditEditingFinished);
    connect(ui->firstPasswordLineEdit, &QLineEdit::textChanged, this, &RegisterWidget::onfirstPasswordLineEditEditingFinished);
    connect(ui->secondPasswordLineEdit, &QLineEdit::textChanged, this, &RegisterWidget::onsecondPasswordLineEditEditingFinished);
    connect(TCPConnect::Instance().get(), &TCPConnect::signalRecvRegisterMessage, this, &RegisterWidget::onMsgHandle);
    connect(TCPConnect::Instance().get(), &TCPConnect::signalConnectFailed, this, &RegisterWidget::onSignalConnectedFailed);
}

void RegisterWidget::initUi()
{
    ui->label->setVisible(false);
    ui->label_2->setVisible(false);
    setWindowFlag(Qt::WindowStaysOnTopHint);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}
