#include "debugwindow.h"
#include "ui_debugwindow.h"
#include <QTime>
#include <QDebug>

DebugWindow::DebugWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Окно отладки"));

  //  connect(ui->x10_pushButton, SIGNAL(clicked(bool)), this, SLOT(onX10_pb()));
  /*  connect(ui->x10_pushButton, SIGNAL(clicked(bool)), this, SLOT(onX10_pb()));
    connect(ui->x20_pushButton, SIGNAL(clicked(bool)), this, SLOT(onX20_pb()));
    connect(ui->x30_pushButton, SIGNAL(clicked(bool)), this, SLOT(onX30_pb()));
    connect(ui->x40_pushButton, SIGNAL(clicked(bool)), this, SLOT(onX40_pb()));
    connect(ui->x50_pushButton, SIGNAL(clicked(bool)), this, SLOT(onX50_pb()));


    ui->treeWidget->setColumnCount(2);
    QStringList headerLabels;
    headerLabels << "Параметр"<< "Значение";
    ui->treeWidget->setHeaderLabels(headerLabels);
    //qDebug() << ui->treeWidget->minimumSizeHint();
    //ui->treeWidget->resize(ui->treeWidget->minimumSizeHint());


    //this->adjustSize();
*/

}

DebugWindow::~DebugWindow()
{
    delete ui;
}

void DebugWindow::onServiceMessage(QString message)
{
    QString str;
    str.append(QTime::currentTime().toString("hh:mm:ss **** "));
    str.append(message);
    str.append("\n");
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    ui->plainTextEdit->insertPlainText(str);

}

void DebugWindow::onIncomingMessage(QByteArray message)
{
    QString str;
    str.append(QTime::currentTime().toString("hh:mm:ss <--- "));
    for(int i = 0 ; i < message.size(); i++)
    {
        str.append("0x"+QString::number((quint8)message[i],16).toUpper().rightJustified(2,'0')+"  ");
    }
    str.append("\n");
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    ui->plainTextEdit->insertPlainText(str);

}

void DebugWindow::onOutgoingMessage(QByteArray message)
{
    QString str;
    str.append(QTime::currentTime().toString("hh:mm:ss ---> "));
    for(int i = 0 ; i < message.size(); i++)
    {
        str.append("0x"+QString::number((quint8)message[i],16).toUpper().rightJustified(2,'0')+"  ");
    }
    str.append("\n");
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    ui->plainTextEdit->insertPlainText(str);

}

void DebugWindow::lightUp(QLabel * label, bool state)
{
    if( state )
    {
       label->setPixmap(QPixmap(":/icon/square1-green.png"));
    }
    else
    {
       label->setPixmap(QPixmap(":/icon/square1-red.png"));
    }
}

QLabel* DebugWindow::getColorLabel(bool state)
{
    QLabel *label = new QLabel;
    if( state )
    {
       label->setPixmap(QPixmap(":/icon/square1-green.png"));
    }
    else
    {
       label->setPixmap(QPixmap(":/icon/square1-red.png"));
    }
    return label;
}



void DebugWindow::on_btnSend_clicked()
{
    emit CustomCommand(ui->inputCommand->text(), ui->inputParam_1->text(), ui->inputParam_2->text());
}
