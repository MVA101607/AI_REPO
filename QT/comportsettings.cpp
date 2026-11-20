#include "comportsettings.h"
#include "ui_comportsettings.h"
#include "mxusapi.h"
#include <QDebug>

ComPortSettings::ComPortSettings(QWidget *parent) : QDialog(parent), ui(new Ui::ComPortSettings)
{
    ui->setupUi(this);
    this->setWindowTitle("Настройки последовательного порта");
    lookForPorts();

    connect(ui->Close_pushButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    //connect(ui->portName_comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onPortSettingsChanged()));
    //connect(ui->baudRate_comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onPortSettingsChanged()));
    //connect(ui->queryTime_spinBox, SIGNAL(valueChanged(int)), this, SLOT(onPortSettingsChanged()));
    connect(ui->setSettings_pushButton, SIGNAL(clicked(bool)), this, SLOT(setPortSettings()));
    connect(ui->portListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onListItemDClicked(QListWidgetItem*)));
    connect(ui->radioButton_4w, SIGNAL(clicked(bool)), this, SLOT(setInterfaceType4w()));
    connect(ui->radioButton_2w, SIGNAL(clicked(bool)), this, SLOT(setInterfaceType2w()));
}

void ComPortSettings::Init(QString port, int baud, int delay)
{
    ui->portName_comboBox->setCurrentText(port);
    ui->baudRate_comboBox->setCurrentText(QString(baud));
    ui->queryTime_spinBox->setValue(delay);
    showInterfaceType();
}

bool ComPortSettings::showInterfaceType(void)
{
    QString port = ui->portName_comboBox->currentText();
    QStringRef sPortNum(&port,3,1);
    int portnum = sPortNum.toInt();
    int result = MxGetInterface(portnum);
    switch(result)
    {
    case(MXU_RS485_4W):
    case(MXU_RS422):
        ui->radioButton_4w->setEnabled(true);
        ui->radioButton_2w->setEnabled(true);
        ui->radioButton_4w->setChecked(true);
        type = "4W";
        break;
    case(MXU_RS485_2W):
        ui->radioButton_4w->setEnabled(true);
        ui->radioButton_2w->setEnabled(true);
        ui->radioButton_2w->setChecked(true);
        type = "2W";
        break;
    case(MXU_RS232):
        ui->radioButton_4w->setEnabled(true);
        ui->radioButton_2w->setEnabled(true);
        ui->radioButton_4w->setChecked(false);
        ui->radioButton_2w->setChecked(false);
        type = "--";
        break;
    default:
        ui->radioButton_4w->setChecked(false);
        ui->radioButton_2w->setChecked(false);
        ui->radioButton_4w->setEnabled(false);
        ui->radioButton_2w->setEnabled(false);
        type = "--";
    }

    //ui->label->setText(QString::number(result));
    return true;
}

void ComPortSettings::setInterfaceType4w(void)
{
    QString port = ui->portName_comboBox->currentText();
    QStringRef sPortNum(&port,3,1);
    int portnum = sPortNum.toInt();
    MxSetInterface(portnum, MXU_RS485_4W);
    showInterfaceType();
}

void ComPortSettings::setInterfaceType2w(void)
{
    QString port = ui->portName_comboBox->currentText();
    QStringRef sPortNum(&port,3,1);
    int portnum = sPortNum.toInt();
    MxSetInterface(portnum, MXU_RS485_2W);
    showInterfaceType();
}

ComPortSettings::~ComPortSettings()
{
    delete ui;
}

void ComPortSettings::lookForPorts()
{
    ui->portListWidget->clear();
    ui->portName_comboBox->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->portListWidget->addItem(info.portName() +"   "+info.description());
        ui->portName_comboBox->addItem(info.portName());
    }

    if (QSerialPortInfo::availablePorts().isEmpty())
    {
        ui->portListWidget->addItem("Нет доступных портов");
        ui->portName_comboBox->addItem("Нет");
    }
}

void ComPortSettings::onPortSettingsChanged()
{
    emit portSettingsChanged(ui->portName_comboBox->currentText(), ui->baudRate_comboBox->currentText(),ui->queryTime_spinBox->value(), type);
}

void ComPortSettings::setPortSettings()
{
    emit portSettingsChanged(ui->portName_comboBox->currentText(), ui->baudRate_comboBox->currentText(),ui->queryTime_spinBox->value(), type);
    close();
}

void ComPortSettings::onListItemDClicked(QListWidgetItem * item)
{
    if (item)
    {
        ui->portName_comboBox->setCurrentIndex(ui->portName_comboBox->findText(item->text().section(' ', 0, 0)));

        emit portSettingsChanged(ui->portName_comboBox->currentText(), ui->baudRate_comboBox->currentText(),ui->queryTime_spinBox->value(), type);

        close();
    }
}

void ComPortSettings::on_portListWidget_itemActivated(QListWidgetItem *item)
{
    ui->portName_comboBox->setCurrentIndex(ui->portName_comboBox->findText(item->text().section(' ', 0, 0)));
    showInterfaceType();
}

void ComPortSettings::on_portName_comboBox_currentTextChanged(const QString &arg1)
{
    showInterfaceType();
}

void ComPortSettings::on_portListWidget_itemClicked(QListWidgetItem *item)
{
    ui->portName_comboBox->setCurrentIndex(ui->portName_comboBox->findText(item->text().section(' ', 0, 0)));
    showInterfaceType();
}
