#include "thermostat.h"
#include "ui_thermostat.h"
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QSettings>
#include <QMessageBox>




thermostat::thermostat(PA_Shell *parent) : QDialog(parent), ui(new Ui::thermostat)
{
  Parent = parent;
  ui->setupUi(this);
  port = new QSerialPort();
  port->setBaudRate(115200);
  port->setDataBits(QSerialPort::DataBits(8));
  port->setStopBits(QSerialPort::StopBits(1));
  port->setParity(QSerialPort::Parity(0));
  //connect(port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
  lookForPorts();
  readSettings();
  try_count = 0;

  connect(ui->horizontalSlider, SIGNAL(sliderReleased()), this, SLOT(slider_process()));
  connect(ui->pushButton_Apply, SIGNAL(clicked(bool)), this, SLOT(writeSettings()));
  connect(ui->lineEdit_Temp, SIGNAL(editingFinished()), this, SLOT(UpdateVariables()));
  connect(ui->lineEdit_Kp, SIGNAL(editingFinished()), this, SLOT(UpdateVariables()));
  connect(ui->lineEdit_Ki, SIGNAL(editingFinished()), this, SLOT(UpdateVariables()));
  connect(ui->lineEdit_Kd, SIGNAL(editingFinished()), this, SLOT(UpdateVariables()));
  connect(ui->lineEdit_Time, SIGNAL(editingFinished()), this, SLOT(UpdateVariables()));

  pidTimer = new QTimer(this);
  connect(pidTimer, SIGNAL(timeout()), this, SLOT(onPidTimer()));
  pidTimer->setInterval(3333);

  blinkTimer = new QTimer(this);
  //connect(blinkTimer, SIGNAL(timeout()), this, SLOT(onOneShotTimer()));

  customPlot = ui->customPlot;
  customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
//    customPlot->legend->setVisible(true);
  QFont legendFont = font();  // start out with MainWindow's font..
  legendFont.setPointSize(9); // and make a bit smaller for legend
  customPlot->legend->setFont(legendFont);
  customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
  //customPlot->xAxis->setLabel("Время");
  //customPlot->yAxis->setLabel("Температура");
  customPlot->yAxis->setRange(0, 100);
  // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
  customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

  customPlot->addGraph();
  customPlot->graph(0)->setPen(QPen(QColor(255, 100, 0)));
  customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
  //customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
  customPlot->graph(0)->setName("Измеренная температура");

  customPlot->addGraph();
  customPlot->graph(1)->setPen(QPen(QColor(120, 120, 250)));
  customPlot->graph(1)->setName("Действие вентиляторов");

  customPlot->addGraph();
  customPlot->graph(2)->setPen(QPen(QColor(50, 200, 50)));
  customPlot->graph(2)->setName("Ошибка");

  /*for (int i=0; i<100; ++i)
  {
    double x = i/5.0 - 10; // x goes from -1 to 1
    xtime.append(x);
    temp.append(x*x);  // let's plot a quadratic function
  }
  tickcount = 100;*/
  customPlot->replot();
  UpdateVariables();

}

void thermostat::MyPortClose(void)
{
    //port->flush();
    port->clear();
    port->close();
    port->reset();
    delete port;
    port = new QSerialPort();
      port->setBaudRate(115200);
      port->setDataBits(QSerialPort::DataBits(8));
      port->setStopBits(QSerialPort::StopBits(1));
      port->setParity(QSerialPort::Parity(0));
}

thermostat::~thermostat(){
    writeSettings();
    pidTimer->stop();
    if(port->isOpen()){
        MyPortClose();
        }
    port->close();
    delete port;
}

void thermostat::onPidTimer()
{
    Temp = Parent->getMaxTemp();
    //Temp = ui->verticalSlider->value();
    double Error = Temp - Temp_ustavka;
    double AvgError = (Parent->getAvgTemp()) - Temp_ustavka;

/*
    //прямая схема ПИД - уровень воздействия накапливается в интеграле
    if(((imax - Error) > integral)&&((Error - imax) < integral))
        integral += Error;
    double P = Kp*Error;
    double I = Ki*integral;
    double D = Kd*(Error - oldError);
    U = P+I+D;*/

    // разностная схема ПИД - уровень воздействия хранится в итоговой переменной
    // интеграл считается как среднее по конечному числу последних ошибок (окну графика)
    integral = 0;
    ibuf.append(Error);
    int width;
    if(ibuf.count() > period) width = period; else width = ibuf.count();
    for(int i = ibuf.count()-width; i<ibuf.count(); i++) integral+=ibuf[i];
    integral /= width;
    double P = Kp*Error;
    double I = Ki*integral;
    double D = Kd*(AvgError - oldError);
    U += P+I+D;
    if(U>100.0) U = 100.0;
    else if(U < 0.0) U = 0.0;

    oldError = AvgError;
    int Out = int(U);
    if(Out < 0) Out = 0;
    if(Out > 100) Out = 100;

    y_out.append(Out);
    int OutAvg = 0;
    foreach(const double &elem, y_out){   OutAvg+=elem;   }
    OutAvg/= y_out.count();
    if(Out < (OutAvg)) Out = (OutAvg);

    SendByte(128+char(Out));

    ui->horizontalSlider->setValue(Out);
    ui->label_CurrentSpeed->setText(QString::number(Out));
    ui->label_CurTemp->setText(QString::number(Temp,'f',1));
    ui->label_Kp->setText(QString::number(P,'f',2));
    ui->label_Ki->setText(QString::number(I,'f',2));
    ui->label_Kd->setText(QString::number(D,'f',2));


    temp.append(Temp);
    xtime.append(tickcount++);
    if(xtime.count() > graphwidth) {
        xtime.removeFirst();
        temp.removeFirst();
        y_out.removeFirst();
        ibuf.removeFirst();
    }
    Draw();
}

void thermostat::Draw(){
    if(ui->customPlot->graphCount()>2){
       ui->customPlot->graph(0)->setData(xtime,temp);
       ui->customPlot->graph(1)->setData(xtime,y_out);
       ui->customPlot->graph(2)->setData(xtime,ibuf);
       ui->customPlot->xAxis->setRange(tickcount-graphwidth, tickcount);
       ui->customPlot->replot();
    }
}


void thermostat::onOneShotTimer(){
    //ui->label_LED->setPixmap(QPixmap(":/icon/square1-white.png"));
    if(port->isOpen()){
        QByteArray data = port->readAll();
        QString c;

        foreach(const char& b, data){
            c.setNum(ushort(b),10);
            dbg_str.prepend(c+" ");
        }

        if(dbg_str.length()>100){
            dbg_str.truncate(100);
        }

        ui->label_DBG->setText(dbg_str);
        port->clear();
        port->reset();
        if(data.size() > 0){
            if(data[data.size()-1] != lastbyte){
                try_count++;
                if(try_count < 10)
                    SendByte(lastbyte);
                else
                {
                    ui->label_LED->setPixmap(QPixmap(":/icon/square1-red.png"));
                    //MyPortClose();
                    try_count = 0;
                }
            }
            else
            {
                try_count = 0;
                ui->label_LED->setPixmap(QPixmap(":/icon/square1-white.png"));
            }
        }
        else
        {
            dbg_str.prepend("x");
            try_count++;
            if(try_count < 10)
                SendByte(lastbyte);
            else {
                ui->label_LED->setPixmap(QPixmap(":/icon/square1-red.png"));
                //MyPortClose();
                try_count = 0;
            }
        }
    }
    else
    {
        try_count++;
        if(try_count < 10)
            SendByte(lastbyte);
        else {
            ui->label_LED->setPixmap(QPixmap(":/icon/square1-red.png"));
            //MyPortClose();
            try_count = 0;
        }
    }
}

void thermostat::UpdateVariables(){

    Temp_ustavka = ui->lineEdit_Temp->text().toDouble();
    if(Temp_ustavka > 100.0) Temp_ustavka = 100.0;
    if(Temp_ustavka < 0.0) Temp_ustavka = 0.0;
    ui->lineEdit_Temp->setText(QString::number(Temp_ustavka,'f',0));

    Kp = ui->lineEdit_Kp->text().toDouble();
    ui->lineEdit_Kp->setText(QString::number(Kp,'f',2));

    Ki = ui->lineEdit_Ki->text().toDouble();
    ui->lineEdit_Ki->setText(QString::number(Ki,'f',2));

    Kd = ui->lineEdit_Kd->text().toDouble();
    ui->lineEdit_Kd->setText(QString::number(Kd,'f',2));

    period = ui->lineEdit_Time->text().toInt();
    if(period > graphwidth) period = graphwidth;
    if(period < 1)   period = 1;
    ui->lineEdit_Time->setText(QString::number(period));
    //pidTimer->setInterval(period*1000);
    //onPidTimer();
}

void thermostat::lookForPorts()
{
    char flag = 0;

    if(port->isOpen())
        MyPortClose();
    if (QSerialPortInfo::availablePorts().isEmpty())
    {
        ui->label_PortName->setText("Не подключен");
        ui->checkBox_PidEnable->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
    }
    else {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            if(info.description().contains("STM"))
            {
                port->setPortName(info.portName());
                if(port->open(QIODevice::ReadWrite)){
                    ui->label_PortName->setText("Порт "+info.portName());
                    ui->checkBox_PidEnable->setEnabled(true);
                    ui->horizontalSlider->setEnabled(true);
                    flag = 1;
                    }
                break;
            }
        }

    }
    if(flag == 0){
        ui->label_PortName->setText("Не подключен");
        ui->checkBox_PidEnable->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
    }
}

void thermostat::writeSettings()
{
    QSettings settings(QString( QDir::currentPath() + "//settings.ini" )  , QSettings::IniFormat);

    settings.sync();

    settings.setValue("therm_Temp", ui->lineEdit_Temp->text());
    settings.setValue("therm_Kp", ui->lineEdit_Kp->text());
    settings.setValue("therm_Ki", ui->lineEdit_Ki->text());
    settings.setValue("therm_Kd", ui->lineEdit_Kd->text());
    settings.setValue("therm_Tau",  ui->lineEdit_Time->text());

    settings.sync();

}

void thermostat::readSettings()
{

    if(QFile::exists(QDir::currentPath() + "//settings.ini"))
    {

        QSettings settings(QString( QDir::currentPath() + "//settings.ini" ), QSettings::IniFormat);

        ui->lineEdit_Temp->setText(settings.value("therm_Temp", "75").toString());
        ui->lineEdit_Kp->setText(settings.value("therm_Kp", "10").toString());
        ui->lineEdit_Ki->setText(settings.value("therm_Ki", "0.1").toString());
        ui->lineEdit_Kd->setText(settings.value("therm_Kd", "2").toString());
        ui->lineEdit_Time->setText(settings.value("therm_Tau", "1").toString());
    }

}

void thermostat::SendByte(char b)
{
    if(ui->label_PortName->text() != "Не подключен"){
        if(!port->isOpen()){
            lookForPorts();
            if(!port->isOpen()){
                ui->label_LED->setPixmap(QPixmap(":/icon/square1-red.png"));
                return;
                }
        }
    QByteArray byteArray;
    byteArray.append(b);
    port->write(byteArray);
    byteArray.clear();
    lastbyte = b;
    ui->label_LED->setPixmap(QPixmap(":/icon/square1-green.png"));
    blinkTimer->singleShot(200, this, SLOT(onOneShotTimer()));
    port->flush();  //--
    port->waitForBytesWritten(100);
    }
}

void thermostat::slider_process()
{
    ui->checkBox_PidEnable->setCheckState(Qt::Unchecked);
    int position = ui->horizontalSlider->value();
    if(position < 3){
        SendByte('0');
    }
    else {
        //SendByte('S');
        SendByte(128+char(position));
    }
}
void thermostat::on_horizontalSlider_sliderMoved(int position)
{
    ui->label_CurrentSpeed->setText(QString::number(position));
}

void thermostat::on_checkBox_PidEnable_stateChanged(int arg1)
{
    if(0 == arg1){
        pidTimer->stop();
    }
    else {
        pidTimer->start();
        onPidTimer();
    }
}


void thermostat::on_pushButton_Connect_pressed()
{
    lookForPorts();
}

void thermostat::on_pushButton_Stop_pressed()
{
    pidTimer->stop();
    ui->checkBox_PidEnable->setChecked(false);
    SendByte('0');
}

void thermostat::on_pushButton_Start_pressed()
{
    pidTimer->start();
    ui->checkBox_PidEnable->setChecked(true);
}

void thermostat::on_pushButton_50_pressed()
{
    SendByte(char(178));
    ui->horizontalSlider->setValue(50);
    ui->label_CurrentSpeed->setText(QString::number(50));
}


void thermostat::on_pushButton_100_pressed()
{
    SendByte(char(228));
    ui->horizontalSlider->setValue(100);
    ui->label_CurrentSpeed->setText(QString::number(100));
}
