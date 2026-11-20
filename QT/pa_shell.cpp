#include "pa_shell.h"
#include "ui_pa_shell.h"
#include "mxusapi.h"
#include "debugwindow.h"
#include "thermostat.h"

#include <QMessageBox>
#include <QTimer>
#include <QDir>
#include <QTime>

#include <QThread>

#define startBytePosition 0
#define deviceAdress 1
#define masterAdress 2
#define dataLengthPosition 3
#define ProgramVersion "2.6.210323"

/*
 *  "2.4.200917" - добавлены кнопки Пуск/Стоп в термостатирование, дифференцирование по средней температуре
 *  "2.5.201013" - убрал постоянное закрытие/открытие порта при термостатировании
 *  "2.6.210323" - поправил форматирование чисел при записи в лог
 */

#define PRO_USER //define if debug information is needed

static thermostat *tstat;

PA_Shell::PA_Shell(QWidget *parent) : QMainWindow(parent), ui(new Ui::PA_Shell)
{
    ui->setupUi(this);



    this->setWindowTitle(tr("Усилитель мощности СВЧ 1000 Вт"));
    this->setWindowIcon(QIcon(":/icon/lightning-bolt-th.png"));

    createActions();
    createMenus();

    QueryTime = 200;
    PortBaudRate = 9600;
    PortName = "COM7";

    port = new QSerialPort(PortName);
    port->setBaudRate(PortBaudRate); //115200 //921600
    port->setDataBits(QSerialPort::DataBits(8));
    port->setStopBits(QSerialPort::StopBits(1));
    port->setParity(QSerialPort::Parity(0)); //QSerialPort::NoParity
    connect(port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));

    connect(ui->turnOn_all_pb, SIGNAL(clicked(bool)), this, SLOT(turnPowerCommandOn()));
    connect(ui->turnOff_all_pb, SIGNAL(clicked(bool)), this, SLOT(turnPowerCommandOff()));
    connect(ui->startQuery_pushButton, SIGNAL(clicked(bool)), this, SLOT(onStartQuery()));
    connect(ui->pbPortCfg, SIGNAL(clicked(bool)), this, SLOT(showPortSettings()));

    connect(ui->turnFirst_pushButton, SIGNAL(clicked(bool)), this, SLOT(turnFirst()));
    connect(ui->turnSecond_pushButton, SIGNAL(clicked(bool)), this, SLOT(turnSecond()));
    connect(ui->turnThird_pushButton, SIGNAL(clicked(bool)), this, SLOT(turnThird()));
    connect(ui->turnFourth_pushButton, SIGNAL(clicked(bool)), this, SLOT(turnFourth()));

    queryTimer = new QTimer(this); // Таймер
    connect(queryTimer, SIGNAL(timeout()), this, SLOT(onQueryTimer()));

    askTimer = new QTimer(this);
    answerTimer = new QTimer(this);

    requestingStatusTimer = new QTimer(this);
    connect(requestingStatusTimer,SIGNAL(timeout()), this, SLOT(noAnswer()));
    requestingStatus = false;


/*    outgoingTimer = new QTimer(this);
    connect(outgoingTimer, &QTimer::timeout, this, &PA_Shell::outgoingQuery);
    outgoingTimer->start(20);*/

    logTimerInterval = 5*60*1000;
    logTimer = new QTimer(this);
    connect(logTimer, SIGNAL(timeout()), this, SLOT(logTimerTask()));
    logTimer->setInterval(logTimerInterval);



    stateMachineChannel = 0;
    noAnswerCounter = 0;
    doLog = 1;

    ui->label_ver->setText(ProgramVersion);

    extendedView(false);
    readSettings();
    createDataTable();

    CenterFlags.Raw = 0;
    int i = 0;
    while(i<5)
    {
        ModuleFlags[i].Raw = 0;
        lastModuleValue[i] = 0;
        dU[i]=dC[i]=dT[i]=dQ[i]=dP[i]=dA[i]=0.0;
        i++;
    }
    if(doLog)
    {
        logFile.setFileName(QString(QDir::currentPath()+"//log.csv"));

        bool fileExist = logFile.exists();

        if(!fileExist)
        {
            logWriteHeader();
        }

        if(logTimerInterval != 0)
        {
            logTimer->start(logTimerInterval);
        }

        //qDebug() << QDir::currentPath();
    }

    tstat = new thermostat(this);
    tstat->setWindowTitle("Управление вентиляторами");

//    ui->pi
}

void PA_Shell::logWriteHeader(void)
{
    if(logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&logFile);
        out << "--time--;" << "-common-;" << "VSWR;" << "Pin;" << "Pout;" << "U380;"
            << "Air1;" << "Air2;"<< "Air3;"<< "Air4;"<< "VSWR;"<< "InP;"  << "OutP;"<< "U380;"
            << "MOD1;"<< "MOD2;" << "MOD3;" << "MOD4;"<< "CRIT;"<< "LOST;"<< "ENAB;"<< "PWRP;"
            << "\n";
        out << "--time--;" << "-module-;" << "Voltage;"<< "Temperature;"  << "Q;"   <<"Tau;"
            << "I1;"   << "I2;"  << "I3;"   << "I4;"  << "I5;"  << "U1;"  << "U2;"  << "U3;"
            << "U4;"  << "U5;"   << "FREQ;" << "VOLT;"<< "TEMP;"<< "ENAB;"<< "ASRT;"<< "RSPN;"
            << "\n";
        out.flush();
        logFile.close();
    }
}


PA_Shell::~PA_Shell()
{
    writeSettings();
    logFile.close();
    port->close();
    delete ui;
    delete port;

}

void PA_Shell::onStartQuery()
{
    if(port->isOpen())
    {
        port->close();
        ui->startQuery_pushButton->setText("Опрос");
        queryTimer->stop();
        portData.clear();
        if(doLog)
        {
            logFile.write(QString("Stop " +QDateTime::currentDateTime().toString("dd.MM.yyyy-hh:mm:ss")+"\n" ).toLatin1().data());
            logFile.close();
        }
        stateMachineChannel = 4;

    }
    else
    {
        if (port->open(QIODevice::ReadWrite))
        {
            /*if(QueryTime !=0)
            {
                stateMachineChannel = 0;
                portData.clear();
                queryTimer->start(QueryTime);
            }*/
            if(doLog)
            {
                logWriteHeader();
                logFile.open(QIODevice::WriteOnly | QIODevice::Append |  QIODevice::Text);
                logFile.write(QString("Start " +QDateTime::currentDateTime().toString("dd.MM.yyyy-hh:mm:ss")+"\n" ).toLatin1().data());
                logFile.flush();
            }
            ui->startQuery_pushButton->setText("Окончить");
            ResetReceiver();
        }
    }

}

void PA_Shell::onSendData(QByteArray byteArray)
{
    ui->ask_label->setPixmap(QPixmap(":/icon/square1-green.png"));
    //answerTimer->singleShot(8000, this, SLOT(redAnswer()));

    //PendingAnswerCode = quint8(byteArray[4]);
    //0x7E проверка данных
    for(int i = 4; i <  byteArray.size(); i++)
    {
        if( (quint8)byteArray[i] == (quint8)0x7E )
        {
            byteArray.insert(i+1,quint8(0x00));
            byteArray[3] = (quint8)byteArray[3]+1;
        }
    }

    // LRC
    quint8 LRC = 0;
    for(int i = 1; i <  byteArray.size(); i++)
    {
        LRC ^= quint8(byteArray[i]);
    }

    byteArray.append((quint8)LRC);
    if((quint8)LRC == (quint8)0x7E)
    {
        byteArray.append(quint8(0x00));
    }

    //Отправка данных
/*    askTimer->singleShot(100,this, SLOT(whiteAsk()));
    if(!requestingStatusTimer->isActive())
    {
        requestingStatusTimer->start(2000);
    }
    requestingStatus = true;*/

    sendBuffer.append(byteArray);
    //port->write(byteArray);
    //emit outgoingMessage(byteArray);
}


void PA_Shell::outgoingQuery()
{
    QByteArray outdata;
    if(!sendBuffer.isEmpty())
    {
       outdata = sendBuffer.takeFirst();
       port->write(outdata);
       emit outgoingMessage(outdata);

       askTimer->singleShot(100,this, SLOT(whiteAsk()));
       if(!requestingStatusTimer->isActive())
       {
           requestingStatusTimer->start(2000);
       }
       requestingStatus = true;
    }
}

void PA_Shell::SendCustomCommand(QString sCommand, QString sParam1, QString sParam2)
{
    quint8 uCommand, uParam1, uParam2;
    bool ok;
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2

    uCommand = quint8(sCommand.toInt(&ok, 0)); //если начинается на 0х - то hex, иначе десятичное
    if(ok)
    {
        uParam1 = quint8(sParam1.toInt(&ok, 0)); //если начинается на 0х - то hex, иначе десятичное
        if(ok)
        {
            uParam2 = quint8(sParam2.toInt(&ok, 0)); //если начинается на 0х - то hex, иначе десятичное
            if(ok)
            {
                byteArray.append((quint8)0x03); //Длинна данных 3
                byteArray.append(uCommand); //Код комманды
                byteArray.append(uParam1);  //параметр 1
                byteArray.append(uParam2);  //параметр 2
            }
            else
            {  // второй параметр отсуствует или не распознался
                byteArray.append((quint8)0x02); //Длинна данных 3
                byteArray.append(uCommand); //Код комманды
                byteArray.append(uParam1);  //параметр 1
            }
        }
        else
        {  // первый параметр отсуствует или не распознался
            byteArray.append((quint8)0x01); //Длинна данных 3
            byteArray.append(uCommand); //Код комманды
        }
        onSendData(byteArray);
    }
    else
    {  // команда отсуствует или ошибка преобразования

    }
}

double PA_Shell::getMaxTemp(){
    double max = 0;
    for(int i=1;i<5;i++) if(max < dC[i]) max = dC[i];  // максимум
    return max;

    /*
    int count = 0;
    for(int i=1;i<5;i++){
        if(dC[i] > 1){
            max+=dC[i];
            count++;
        }
    }
    if(count > 0)
        return max/count;
    else
        return 0;*/
}

double PA_Shell::getAvgTemp(){
    double max = 0;
    int count = 0;
    for(int i=1;i<5;i++){
        if(dC[i] > 1){
            max+=dC[i];
            count++;
        }
    }
    if(count > 0)
        return max/count;
    else
        return 0;
}

void PA_Shell::turnPowerCommand()
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x02); //Длинна данных 3
    byteArray.append((quint8)0x10); //Код комманды
    byteArray.append((quint8)0x01); //байт управления

    onSendData(byteArray);
}

void PA_Shell::turnPowerCommand(bool state)
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x02); //Длинна данных 3
    byteArray.append((quint8)0x10); //Код комманды
    if(state)
    {
        byteArray.append((quint8)0x01); //байт управления
    }
    else
    {
        byteArray.append(quint8(0x00));
    }

    onSendData(byteArray);
}

void PA_Shell::turnSingleModuleCommand(int number)
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x03); //Длинна данных 3
    byteArray.append((quint8)0x15); //Код комманды
    byteArray.append((quint8)number); //номер модуля

    if(ModuleFlags[number].flags.Enab)
    {
        byteArray.append(quint8(0x01)); //байт управления
    }
    else
    {
        byteArray.append(quint8(0x00));
    }

    onSendData(byteArray);
}

void PA_Shell::turnPowerCommandOn()
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x02); //Длинна данных 3
    byteArray.append((quint8)0x10); //Код комманды
    byteArray.append((quint8)0x01); //байт управления
    onSendData(byteArray);
    ResetReceiver();
}

void PA_Shell::turnPowerCommandOff()
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x02); //Длинна данных 3
    byteArray.append((quint8)0x10); //Код комманды
    byteArray.append((quint8)0x00);
    onSendData(byteArray);
    ResetReceiver();
}


void PA_Shell::changeBaudRateCommand(int baudRate)
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x04); //Длинна 3
    byteArray.append((quint8)0x20); //комманда

    byteArray.append( ((quint8)baudRate) ); //комманда
    byteArray.append( (quint8)(baudRate>>8) ); //комманда
    byteArray.append( (quint8)(baudRate>>16) ); //комманда

    onSendData(byteArray);
}

void PA_Shell::getModuleDataCommand(int channel)
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x02); //Длинна 3
    byteArray.append((quint8)0x30); //Код комманды
    byteArray.append((quint8)channel); //Номер модуля
    RequestChannelNumber = quint8(channel);
    onSendData(byteArray);

}


void PA_Shell::getModuleServiceDataCommand(int moduleNumber)
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x02); //Длинна 3
    byteArray.append((quint8)0x50); //Код комманды
    byteArray.append((quint8)moduleNumber); //Номер модуля

    onSendData(byteArray);

}

void PA_Shell::getWholePaDataCommand()
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x01); //Длинна 3
    byteArray.append((quint8)0x40); //комманда

    onSendData(byteArray);

}

void PA_Shell::getWholePaServiceDataCommand()
{
    QByteArray byteArray;
    byteArray.append((quint8)0x7E); //Стартовый байт 0
    byteArray.append((quint8)0x03); //Адрес устройства 1
    byteArray.append((quint8)0x01); //Адрес мастера 2
    byteArray.append((quint8)0x01); //Длинна 3
    byteArray.append((quint8)0x60); //комманда

    onSendData(byteArray);

}

void PA_Shell::FillFormWithModuleData(int chan) // Расставляем полученные данные по местам
{
    QStringList horizontalHeaderLabels;
    double fU = dU[chan];
    double fC = dC[chan];
    double fT = dT[chan];
    double fQ = dQ[chan];
    double fP = dP[chan];
    double fA = dA[chan];

    Module_Data_Union_Type u;
    u.Raw = ModuleFlags[chan].Raw;

    if((doLog == 1)&&(chan > 0))
    {
        if(!logFile.isOpen())
        {
            logFile.open(QIODevice::WriteOnly | QIODevice::Append |  QIODevice::Text);
        }
        if(lastModuleValue[chan] != u.Raw )
        {
            lastModuleValue[chan] = u.Raw;
            QTextStream out(&logFile);
            out <<  QTime::currentTime().toString("hh:mm:ss")   << ";"
                 << "module:" << QString::number(chan)          << ";"
                 << QString::number(dU[chan],'f',1)             << ";"
                 << QString::number(dC[chan],'f',1)             << ";"
                 << QString::number(dQ[chan],'f',1)             << ";"
                 << QString::number(dT[chan],'f',1)             << ";"
                 << QString::number(u.flags.I1)                 << ";"
                 << QString::number(u.flags.I2)                 << ";"
                 << QString::number(u.flags.I3)                 << ";"
                 << QString::number(u.flags.I4)                 << ";"
                 << QString::number(u.flags.I5)                 << ";"
                 << QString::number(u.flags.U1)                 << ";"
                 << QString::number(u.flags.U2)                 << ";"
                 << QString::number(u.flags.U3)                 << ";"
                 << QString::number(u.flags.U4)                 << ";"
                 << QString::number(u.flags.U5)                 << ";"
                 << QString::number(u.flags.Freq)               << ";"
                 << QString::number(u.flags.Volt)               << ";"
                 << QString::number(u.flags.Temp)               << ";"
                 << QString::number(u.flags.Enab)               << ";"
                 << QString::number(u.flags.Assert)             << ";"
                 << QString::number(u.flags.Respond)            << ";"
                 << "\n";
            out.flush();
        }

    }

    if(chan > 0)
    {
        ui->label_tau->setText(QString::number(fT,'f',1));
        ui->label_Q->setText(QString::number( fQ,'f',1));
        ui->label_T->setText(QString::number(fT*fQ,'f',1));
    }

    switch(chan)
    {
    case(1):

        ui->label_ch1u->setText(QString::number(fU,'f',1));
        ui->label_temp1->setText( QString::number( fC,'f',1));
        ui->label_Pout_1->setText(QString::number(fP,'f',1));
        ui->label_Id_1->setText( QString::number( fA,'f',1));

        lightUp(ui->label_led_ch1Isen_4, u.flags.I4 == 0);
        lightUp(ui->label_led_ch1Isen_5, u.flags.I5 == 0);
        lightUp(ui->label_led_ch1Vg_1, u.flags.U1 == 0);
        lightUp(ui->label_led_ch1Vg_2, u.flags.U2 == 0);
        lightUp(ui->label_led_ch1Vg_3, u.flags.U3 == 0);
        lightUp(ui->label_led_ch1Vg_4, u.flags.U4 == 0);
        lightUp(ui->label_led_ch1Vg_5, u.flags.U5 == 0);
        lightUp(ui->t1_label, u.flags.Freq == 0);
        lightUp(ui->U1_label, u.flags.Volt == 0);
        lightUp(ui->C1_label, u.flags.Temp == 0);
        lightUp(ui->power1_label, u.flags.Enab == 0);
        lightUp(ui->label_led_ch1Block, u.flags.Assert == 0);
        lightUp(ui->avail1_label, u.flags.Respond == 0);

        horizontalHeaderLabels.clear();
        horizontalHeaderLabels <<QString::number(u.flags.I1) << QString::number(u.flags.I2) << QString::number(u.flags.I3)
                              << QString::number(u.flags.I4) << QString::number(u.flags.I5)
                              << QString::number(u.flags.U1) << QString::number(u.flags.U2)<< QString::number(u.flags.U3)
                              << QString::number(u.flags.U4) << QString::number(u.flags.U5)
                              << QString::number(u.flags.Freq) << QString::number(u.flags.Volt)<< QString::number(u.flags.Temp)
                              << QString::number(u.flags.Enab)<< QString::number(u.flags.Assert) << QString::number(u.flags.Respond)
                              << QString::number(fU)<< QString::number(fC) << QString::number(fQ) << QString::number(fT);

        for(int i = 0; i < 20; i++)
        {
           Table->setItem( 1 , 1+i , new QTableWidgetItem( horizontalHeaderLabels[i] ) );

           if( i < 16)
           {
                if( ((QString)horizontalHeaderLabels[i]).toInt() != 0)
                    Table->item(1, 1+i)->setBackground(Qt::red);
                else
                    Table->item(1, 1+i)->setBackground(Qt::green);
           }
        }
        break;

    case(2):

        ui->label_ch2u->setText(QString::number(fU,'f',1));
        ui->label_temp2->setText( QString::number( fC,'f',1));
        ui->label_Pout_2->setText(QString::number(fP,'f',1));
        ui->label_Id_2->setText( QString::number( fA,'f',1));

        lightUp(ui->label_led_ch2Isen_4, u.flags.I4 == 0);
        lightUp(ui->label_led_ch2Isen_5, u.flags.I5 == 0);
        lightUp(ui->label_led_ch2Vg_1, u.flags.U1 == 0);
        lightUp(ui->label_led_ch2Vg_2, u.flags.U2 == 0);
        lightUp(ui->label_led_ch2Vg_3, u.flags.U3 == 0);
        lightUp(ui->label_led_ch2Vg_4, u.flags.U4 == 0);
        lightUp(ui->label_led_ch2Vg_5, u.flags.U5 == 0);
        lightUp(ui->t2_label, u.flags.Freq == 0);
        lightUp(ui->U2_label, u.flags.Volt == 0);
        lightUp(ui->C2_label, u.flags.Temp == 0);
        lightUp(ui->power2_label, u.flags.Enab == 0);
        lightUp(ui->label_led_ch2Block, u.flags.Assert == 0);
        lightUp(ui->avail2_label, u.flags.Respond == 0);

        horizontalHeaderLabels.clear();
        horizontalHeaderLabels <<QString::number(u.flags.I1) << QString::number(u.flags.I2) << QString::number(u.flags.I3)
                              << QString::number(u.flags.I4) << QString::number(u.flags.I5)
                              << QString::number(u.flags.U1) << QString::number(u.flags.U2)<< QString::number(u.flags.U3)
                              << QString::number(u.flags.U4) << QString::number(u.flags.U5)
                              << QString::number(u.flags.Freq) << QString::number(u.flags.Volt)<< QString::number(u.flags.Temp)
                              << QString::number(u.flags.Enab)<< QString::number(u.flags.Assert) << QString::number(u.flags.Respond)
                              << QString::number(fU)<< QString::number(fC) << QString::number(fQ) << QString::number(fT);

        for(int i = 0; i < 20; i++)
        {
           Table->setItem( 2 , 1+i , new QTableWidgetItem( horizontalHeaderLabels[i] ) );

           if( i < 16)
           {
                if( ((QString)horizontalHeaderLabels[i]).toInt() != 0)
                    Table->item(2, 1+i)->setBackground(Qt::red);
                else
                    Table->item(2, 1+i)->setBackground(Qt::green);
           }
        }
        break;

    case(3):

        ui->label_ch3u->setText(QString::number(fU,'f',1));
        ui->label_temp3->setText( QString::number( fC,'f',1));
        ui->label_Pout_3->setText(QString::number(fP,'f',1));
        ui->label_Id_3->setText( QString::number( fA,'f',1));

        lightUp(ui->label_led_ch3Isen_4, u.flags.I4 == 0);
        lightUp(ui->label_led_ch3Isen_5, u.flags.I5 == 0);
        lightUp(ui->label_led_ch3Vg_1, u.flags.U1 == 0);
        lightUp(ui->label_led_ch3Vg_2, u.flags.U2 == 0);
        lightUp(ui->label_led_ch3Vg_3, u.flags.U3 == 0);
        lightUp(ui->label_led_ch3Vg_4, u.flags.U4 == 0);
        lightUp(ui->label_led_ch3Vg_5, u.flags.U5 == 0);
        lightUp(ui->t3_label, u.flags.Freq == 0);
        lightUp(ui->U3_label, u.flags.Volt == 0);
        lightUp(ui->C3_label, u.flags.Temp == 0);
        lightUp(ui->power3_label, u.flags.Enab == 0);
        lightUp(ui->label_led_ch3Block, u.flags.Assert == 0);
        lightUp(ui->avail3_label, u.flags.Respond == 0);

        horizontalHeaderLabels.clear();
        horizontalHeaderLabels <<QString::number(u.flags.I1) << QString::number(u.flags.I2) << QString::number(u.flags.I3)
                              << QString::number(u.flags.I4) << QString::number(u.flags.I5)
                              << QString::number(u.flags.U1) << QString::number(u.flags.U2)<< QString::number(u.flags.U3)
                              << QString::number(u.flags.U4) << QString::number(u.flags.U5)
                              << QString::number(u.flags.Freq) << QString::number(u.flags.Volt)<< QString::number(u.flags.Temp)
                              << QString::number(u.flags.Enab)<< QString::number(u.flags.Assert) << QString::number(u.flags.Respond)
                              << QString::number(fU)<< QString::number(fC) << QString::number(fQ) << QString::number(fT);

        for(int i = 0; i < 20; i++)
        {
           Table->setItem( 3 , 1+i , new QTableWidgetItem( horizontalHeaderLabels[i] ) );

           if( i < 16)
           {
                if( ((QString)horizontalHeaderLabels[i]).toInt() != 0)
                    Table->item(3, 1+i)->setBackground(Qt::red);
                else
                    Table->item(3, 1+i)->setBackground(Qt::green);
           }
        }
        break;
    case(4):

        ui->label_ch4u->setText(QString::number(fU,'f',1));
        ui->label_temp4->setText( QString::number( fC,'f',1));
        ui->label_Pout_4->setText(QString::number(fP,'f',1));
        ui->label_Id_4->setText( QString::number( fA,'f',1));

        lightUp(ui->label_led_ch4Isen_4, u.flags.I4 == 0);
        lightUp(ui->label_led_ch4Isen_5, u.flags.I5 == 0);
        lightUp(ui->label_led_ch4Vg_1, u.flags.U1 == 0);
        lightUp(ui->label_led_ch4Vg_2, u.flags.U2 == 0);
        lightUp(ui->label_led_ch4Vg_3, u.flags.U3 == 0);
        lightUp(ui->label_led_ch4Vg_4, u.flags.U4 == 0);
        lightUp(ui->label_led_ch4Vg_5, u.flags.U5 == 0);
        lightUp(ui->t4_label, u.flags.Freq == 0);
        lightUp(ui->U4_label, u.flags.Volt == 0);
        lightUp(ui->C4_label, u.flags.Temp == 0);
        lightUp(ui->power4_label, u.flags.Enab == 0);
        lightUp(ui->label_led_ch4Block, u.flags.Assert == 0);
        lightUp(ui->avail4_label, u.flags.Respond == 0);

        horizontalHeaderLabels.clear();
        horizontalHeaderLabels <<QString::number(u.flags.I1) << QString::number(u.flags.I2) << QString::number(u.flags.I3)
                              << QString::number(u.flags.I4) << QString::number(u.flags.I5)
                              << QString::number(u.flags.U1) << QString::number(u.flags.U2)<< QString::number(u.flags.U3)
                              << QString::number(u.flags.U4) << QString::number(u.flags.U5)
                              << QString::number(u.flags.Freq) << QString::number(u.flags.Volt)<< QString::number(u.flags.Temp)
                              << QString::number(u.flags.Enab)<< QString::number(u.flags.Assert) << QString::number(u.flags.Respond)
                              << QString::number(fU)<< QString::number(fC) << QString::number(fQ) << QString::number(fT);

        for(int i = 0; i < 20; i++)
        {
           Table->setItem( 4 , 1+i , new QTableWidgetItem( horizontalHeaderLabels[i] ) );

           if( i < 16)
           {
                if( ((QString)horizontalHeaderLabels[i]).toInt() != 0)
                    Table->item(4, 1+i)->setBackground(Qt::red);
                else
                    Table->item(4, 1+i)->setBackground(Qt::green);
           }
        }
        break;

    case(0):   // центральный блок

        if(doLog == 1)
        {
            if(lastModuleValue[0] != CenterFlags.Raw) //5 is for common
            {
                if(!logFile.isOpen())
                {
                    logFile.open(QIODevice::WriteOnly | QIODevice::Append |  QIODevice::Text);
                }
                QTextStream out(&logFile);
                out <<  QTime::currentTime().toString("hh:mm:ss") <<";"
                     << "center:" << ";"
                     << QString::number(fVSWR,'f',1) << ";"
                     << QString::number(fPin, 'f',1) << ";"
                     << QString::number(fPout,'f',1) << ";"
                     << QString::number(fU380,'f',1) << ";"
                     << QString::number(CenterFlags.flags.air1) << ";"
                     << QString::number(CenterFlags.flags.air2) << ";"
                     << QString::number(CenterFlags.flags.air3) << ";"
                     << QString::number(CenterFlags.flags.air4) << ";"
                     << QString::number(CenterFlags.flags.vswr) << ";"
                     << QString::number(CenterFlags.flags.pin) << ";"
                     << QString::number(CenterFlags.flags.pout) << ";"
                     << QString::number(CenterFlags.flags.u380) << ";"
                     << QString::number(CenterFlags.flags.emergency1) << ";"
                     << QString::number(CenterFlags.flags.emergency2) << ";"
                     << QString::number(CenterFlags.flags.emergency3) << ";"
                     << QString::number(CenterFlags.flags.emergency4) << ";"
                     << QString::number(CenterFlags.flags.Critical) << ";"
                     << QString::number(CenterFlags.flags.Pulse_Lost) << ";"
                     << QString::number(CenterFlags.flags.Enab) << ";"
                     << QString::number(CenterFlags.flags.Power_Present) << ";"
                     << "\n";

                out.flush();
            }
            lastModuleValue[0] = CenterFlags.Raw;
        }

        ui->label_Pind->setText(QString::number(fPin,'f',1));
        ui->label_Poutd->setText(QString::number(fPout,'f',1));
        ui->label_vswr->setText(QString::number(fVSWR,'f',2));
        ui->label_u380->setText( QString::number(fU380, 'f',1));

        if(fPout > 100.0)
            ui->label_Pout_name->setText("Pвых, Вт");
        else
            if(fPout > 1.0)
                ui->label_Pout_name->setText("Pвых, дБм");

        lightUp(ui->vswr_label, CenterFlags.flags.vswr == 0);
        lightUp(ui->Pi_label, CenterFlags.flags.pin == 0);
        lightUp(ui->Po_label, CenterFlags.flags.pout == 0);
        lightUp(ui->u380_label, CenterFlags.flags.u380 == 0);
        lightUp(ui->Critical_label, CenterFlags.flags.Critical == 0);
        lightUp(ui->cool_label, (CenterFlags.flags.air1 || CenterFlags.flags.air2 || CenterFlags.flags.air3 || CenterFlags.flags.air4) == 0);
        lightUp(ui->lost_label, CenterFlags.flags.Pulse_Lost == 0);
        lightUp(ui->power0_label, CenterFlags.flags.Enab == 1);
        lightUp(ui->present_label, CenterFlags.flags.Power_Present == 1);

        horizontalHeaderLabels.clear();
        horizontalHeaderLabels <<QString::number(CenterFlags.flags.air1)
                              << QString::number(CenterFlags.flags.air2)
                              << QString::number(CenterFlags.flags.air3)
                              << QString::number(CenterFlags.flags.air4)
                              << QString::number(CenterFlags.flags.vswr)
                              << QString::number(CenterFlags.flags.pin)
                              << QString::number(CenterFlags.flags.pout)
                              << QString::number(CenterFlags.flags.u380)
                              << QString::number(CenterFlags.flags.emergency1)
                              << QString::number(CenterFlags.flags.emergency1)
                              << QString::number(CenterFlags.flags.emergency3)
                              << QString::number(CenterFlags.flags.emergency4)
                              << QString::number(CenterFlags.flags.Critical)
                              << QString::number(CenterFlags.flags.Pulse_Lost)
                              << QString::number(CenterFlags.flags.Enab)
                              << QString::number(CenterFlags.flags.Power_Present)
                              << QString::number(fVSWR)
                              << QString::number(fPin)
                              << QString::number(fPout)
                              << QString::number(fU380);

        for(int i = 0; i < 20; i++)
        {
           Table->setItem( 7 , 1+i , new QTableWidgetItem( horizontalHeaderLabels[i] ) );
           if( i < 16)
           {
                if( ((QString)horizontalHeaderLabels[i]).toInt() != 0)
                {
                    Table->item(7, 1+i)->setBackground(Qt::red);
                }
                else
                {
                    Table->item(7, 1+i)->setBackground(Qt::green);
                }
           }
        }

        break;
    }

}

void PA_Shell::onDataAvailable() // Прием данных
{
    QByteArray data = port->readAll();
    portData.append(data);
    emit incomingMessage(portData);
    ProcessIncomingData();
}

bool PA_Shell::ProcessIncomingData()
{
    bool startReceived = false;
    quint8 dataLength = 0;
    quint8 constDataLength = 0;
    quint8 LRC = 0;

    // старт-пакет это три байта 7E 01 03
    while (!startReceived && portData.size() >= 3)
    {
        while(portData.size() > 0)
        {
            if(quint8(portData[0]) == 0x7E) break;
            portData.remove(0,1);
        }

        // если длина данных не нулевая - то первый символ 7E
        if(portData.size() >= 3){
/*            if(quint8(portData[0]) == 0x7E)
            {
                if((quint8)portData[1] != 0x01  || (quint8)portData[2] != 0x03)
                {
                    portData.remove(0,1);
                }
            }*/

            if( (quint8(portData[1]) == 0x01) && (quint8(portData[2]) == 0x03) )
                startReceived = true;
            else
                portData.remove(0,1); // удаляем 7E, если за ним не ожидаемые нами символы
        }
    }

    if(startReceived && portData.size() >= 4)
    {
        dataLength = quint8(portData[3])+1;
        constDataLength = dataLength;
        if(portData.size() >= (4 + dataLength))
        {

            LRC = 0;
            for(int i = 1; i < dataLength+4; i++ )
            {
                LRC ^= (quint8)portData[i];
            }

            if(LRC == (quint8)portData[dataLength+4]) // Если контрольная сумма совпадает, то обрабатываются данные
            {
emit serviceMessage("LRC OK");
                ui->answer_label->setPixmap(QPixmap(":/icon/square1-green.png"));
                answerTimer->singleShot(100,this, SLOT(whiteAnswer()));
               // AnswerCode = quint8(portData[4]);

                for(int i = 4; i <  dataLength+4; i++)
                {
                    if( (quint8)portData[i] == (quint8)0x7E  )
                    {
                        portData.remove(i+1,1);
                        dataLength--;
                    }
                }


                if((quint8)portData[4] == 0x10)
                {
                    //Вкл
                }

                if((quint8)portData[4] == 0x20)
                { 
                    if((quint8)portData[5] == 0x00)
                    {
                        //Скорость
                        statusBar()->showMessage(QString("Скорость изменилась"), 400);
                    }
                    else
                    {
                        statusBar()->showMessage(QString("Ошибка изменения скорости"), 400);
                    }

                }


                if((quint8)portData[4] == 0x30)
                {
                    //Данные с модуля
                    quint16 Up,Q,T;
                    qint16 C;

                    Up = quint16(quint8(portData[5]) | (quint8(portData[6]) << 8));
                    C = qint16(quint8(portData[7])  | (quint8(portData[8]) << 8));
                    Q = (quint8)portData[9] | (quint8(portData[10]) << 8);
                    T = (quint8)portData[11] | (quint8(portData[12]) << 8);
                    ModuleFlags[RequestChannelNumber].Raw = (quint8)portData[13] | ((quint8)portData[14] << 8);

                    dU[RequestChannelNumber] = double(Up)/ 64.0;
                    dC[RequestChannelNumber] = double(C) / 64.0;
                    dT[RequestChannelNumber] = double(T) / 64.0;
                    dQ[RequestChannelNumber] = double(Q) / 64.0;

                    //if(0 == Q) fQ = 0.0001;

                    FillFormWithModuleData(RequestChannelNumber);
                }




                if((quint8)portData[4] == 0x40) //Common //Общий модуль //Большой модуль
                {
                    quint16 VSWR,Pin,Pout,U380;


                    VSWR = (quint8)portData[5] | ((quint8)portData[6] << 8);
                    Pin = (quint8)portData[7] | ((quint8)portData[8] << 8);
                    Pout = (quint8)portData[9] | ((quint8)portData[10] << 8);
                    U380 = (quint8)portData[11] | ((quint8)portData[12] << 8);
                    CenterFlags.Raw = (quint8)portData[13] | ((quint8)portData[14] << 8);

                    fVSWR = double(VSWR) / 64.0;
                    fPin  =  double(Pin) / 64.0;
                    fPout = double(Pout) / 64.0;
                    fU380 = double(U380) / 64.0;

                    FillFormWithModuleData(0);
                }

                if((quint8)portData[4] == 0x50) // service data
                {
                    quint16 Up,Q,T,P,A;
                    qint16 C;

                    quint8 moduleNum = 0;
                    moduleNum = (quint8)portData[5];

                    Up = (quint8)portData[6] | ((quint8)portData[7] << 8);
                    C = (qint16) ((quint8)portData[8] | ((quint8)portData[9] << 8)) ;
                    Q = (quint8)portData[10] | ((quint8)portData[11] << 8);
                    T = (quint8)portData[12] | ((quint8)portData[13] << 8);
                    P = (quint8)portData[14] | ((quint8)portData[15] << 8);
                    A = (quint8)portData[16] | ((quint8)portData[17] << 8);
                    ModuleFlags[moduleNum].Raw = (quint8)portData[18] | ((quint8)portData[19] << 8);

                    dU[moduleNum] = double(Up);
                    dC[moduleNum] = double(C) ;
                    dT[moduleNum] = double(T) ;
                    dQ[moduleNum] = double(Q) ;
                    dP[moduleNum] = double(P) ;
                    dA[moduleNum] = double(A) ;


                    FillFormWithModuleData(moduleNum);
                }

                if((quint8)portData[4] == 0x60) // service data
                {
                    quint32 VSWR,Pin,Pout,U380;

                    VSWR = (quint8)portData[5] | ((quint8)portData[6] << 8);
                    Pin = (quint8)portData[7] | ((quint8)portData[8] << 8);
                    Pout = (quint8)portData[9] | ((quint8)portData[10] << 8);
                    U380 = (quint8)portData[11] | ((quint8)portData[12] << 8);
                    CenterFlags.Raw  = (quint8)portData[13] | ((quint8)portData[14] << 8);

                    fVSWR = (double)VSWR;
                    fPin = (double)Pin;
                    fPout = (double)Pout;
                    fU380 = (double)U380;

                    FillFormWithModuleData(0);
                }


                requestingStatusTimer->stop();
                noAnswerCounter = 0;
                requestingStatus = false;
                statusBar()->showMessage(QString("Данные приняты"), 400);
                //portData.clear();
                portData.remove(0,5 + constDataLength);
                if(doLog)
                {
                    logFile.flush();
                }

            }
            else {
                ui->answer_label->setPixmap(QPixmap(":/icon/square1-red.png"));
                emit serviceMessage("LRC BAD");
                portData.remove(0,3); // удаляем стартовый пакет, который привел к плохой сумме
            }
        }

    }

   // emit incomingMessage(portData);

  if(portData.size() > 3)
      return true;
  else
      return false;
}

void PA_Shell::uartDataProcess(QByteArray data)
{
    Q_UNUSED(data);
}

void PA_Shell::queryCycle()
{

}

void PA_Shell::onPortSettingsChanged(QString name, QString baudrate, int queryTime, QString type)
{
    queryTimer->stop();

    PortBaudRate = baudrate.toInt();
    QueryTime = queryTime;
    PortName = name;

    ui->comPortState_label->setText(QString( PortName  + "/" + QString::number(PortBaudRate)) + "/" + type );

    if(port->isOpen())
    {
        changeBaudRateCommand(PortBaudRate);

        port->close();
        port->setPortName(PortName);
        port->setBaudRate(PortBaudRate);
        port->open(QIODevice::ReadWrite);
        if(QueryTime !=0)
        {
            queryTimer->start(QueryTime);
        }
    }
    else
    {
        port->setPortName(PortName);
        port->setBaudRate(PortBaudRate);
    }

}

void PA_Shell::createActions()
{
    startAction = new QAction(tr("Старт"), this);
    startAction->setIcon(QIcon(":/icon/Start-icon.png"));
    startAction->setStatusTip(tr("Старт"));

    showPortSettingsAction = new QAction(tr("Настойки COM порта"), this);
    showPortSettingsAction->setStatusTip(tr("Открыть настойки последовательного порта"));
    connect(showPortSettingsAction, SIGNAL(triggered(bool)), this, SLOT(showPortSettings()));

    showHelpAction = new QAction(tr("Как работать"), this);
    showHelpAction->setIcon(QIcon(":/icon/Help-icon.png"));
    showHelpAction->setStatusTip(tr("Как работать с программой"));
    connect(showHelpAction, SIGNAL(triggered(bool)), this, SLOT(showHelp()));

    debugModeAction = new QAction(tr("Режим отладки"), this);
    debugModeAction->setStatusTip(tr("Вкл/выкл режим отладки"));
    //debugModeAction->setCheckable(true);
    connect(debugModeAction, SIGNAL(triggered(bool)), this, SLOT(showDebugWindow()));

    showThermostatAction = new QAction(tr("Термостатирование"), this);
    connect(showThermostatAction, SIGNAL(triggered(bool)), this, SLOT(showThermostat()));

    showAboutAction = new QAction(tr("О программе"), this);
    connect(showAboutAction, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

    /*extendedViewAction = new QAction(tr("Расширенный режим"), this);
    extendedViewAction->setCheckable(true);
    extendedViewAction->setStatusTip(tr("Показывать все возможные параметры и флаги"));
    connect(extendedViewAction, SIGNAL(toggled(bool)), this, SLOT(extendedView(bool)));*/

    serviceRequestAction = new QAction(tr("Сервисный запрос"), this);
    serviceRequestAction->setCheckable(true);
    serviceRequestAction->setStatusTip(tr("Отправка запроса 0x50 и 0x60 вместо 0x30 и 0x40 соответственно"));
    connect(serviceRequestAction, SIGNAL(toggled(bool)), this, SLOT(extendedView(bool)));

    showDataTableAction = new QAction(tr("Таблица данных"), this);
    //showDataTableAction->setCheckable(true);
    showDataTableAction->setStatusTip(tr("Показать таблицу со значениями битов посылок"));
    //connect(showDataTableAction, SIGNAL(toggled(bool)), this, SLOT(showDataTable(bool)));
    connect(showDataTableAction, SIGNAL(triggered(bool)), this, SLOT(showDataTable(bool)));



}

void PA_Shell::createMenus()
{
  //  settingsMenu = menuBar()->addMenu(tr("Настройки"));
  //  settingsMenu->addAction(showPortSettingsAction);



    //viewMenu =  menuBar()->addMenu(tr("Вид"));
    viewMenu = new QMenu(this);
    viewMenu->addAction(showPortSettingsAction);
    viewMenu->addAction(debugModeAction);
//    viewMenu->addAction(extendedViewAction);
    viewMenu->addAction(serviceRequestAction);
    viewMenu->addAction(showDataTableAction);
//    viewMenu->menuAction()->setVisible(0);

/*    helpMenu = menuBar()->addMenu(tr("Справка"));
    helpMenu->addAction(showHelpAction);
    helpMenu->addAction(showAboutAction);*/
    viewMenu->addAction(showThermostatAction);
    viewMenu->addAction(showHelpAction);
    viewMenu->addAction(showAboutAction);


}

void PA_Shell::showPortSettings()
{
    ComPortSettings *sd = new ComPortSettings(this);
    //connect(sd, SIGNAL(applySettingsSignal(QHash<QString,QString>)), this, SLOT(applySettings(QHash<QString,QString>)));
    connect(sd, SIGNAL(portSettingsChanged(QString,QString,int,QString)), this, SLOT(onPortSettingsChanged(QString,QString,int,QString)));
    sd->Init(PortName, PortBaudRate, QueryTime);
    sd->show();
}

void PA_Shell::showHelp()
{
   QDialog *dialog = new QDialog(this);
   Ui::HelpDialog helpUi;
   helpUi.setupUi(dialog);
   dialog->setWindowTitle("Помощь");
   dialog->show();
}

void PA_Shell::showThermostat()
{
    //thermostat *tstat = new thermostat(this);
    //tstat->setWindowTitle("Управление вентиляторами");
    tstat->show();
}

void PA_Shell::showDebugWindow()
{

    DebugWindow *dbgWndw = new DebugWindow(this);


    connect(dbgWndw, SIGNAL(CustomCommand(QString,QString,QString)), this, SLOT(SendCustomCommand(QString,QString,QString)));
 /*   connect(dbgWndw, SIGNAL(x20_signal()), this, SLOT(changeBaudRateCommand()));
    connect(dbgWndw, SIGNAL(x30_signal()), this, SLOT(getModuleDataCommand()));
    connect(dbgWndw, SIGNAL(x40_signal()), this, SLOT(getWholePaDataCommand()));
    connect(dbgWndw, SIGNAL(x50_signal()), this, SLOT(getModuleServiceDataCommand()));*/
    connect(this, SIGNAL(incomingMessage(QByteArray)), dbgWndw, SLOT(onIncomingMessage(QByteArray)));
    connect(this, SIGNAL(outgoingMessage(QByteArray)), dbgWndw, SLOT(onOutgoingMessage(QByteArray)));
    connect(this, SIGNAL(serviceMessage(QString)),  dbgWndw, SLOT(onServiceMessage(QString)));
    //connect(this, SIGNAL(rawDataMessage(QHash<QString,quint16>)), dbgWndw, SLOT(onRawDataMessage(QHash<QString,quint16>)));

    dbgWndw->show();

}

void PA_Shell::createDataTable()
{

    Table = new QTableWidget;
    Table->setColumnCount(21);
    Table->setRowCount(8);

    QStringList horizontalHeaderLabels;

    horizontalHeaderLabels <<"Знач" << "I1" << "I2"<< "I3" << "I4"<< "I5" << "U1"<< "U2" << "U3"<< "U4" << "U5"<< "Freq" << "Volt"<< "Temp" << "Enab"<< "Block" << "Avail" << "Up"<< "C"<< "Q"<< "T";


    for(int i = 0; i < 21; i++)
    {
       Table->setItem( 0 , i , new QTableWidgetItem( horizontalHeaderLabels[i] ) );
    }


    QStringList verticalHeaderLabels;
    verticalHeaderLabels << "УМ1" << "УМ2" << "УМ3" << "УМ4";

    for(int j = 0; j < 4; j++)
    {
       Table->setItem( 1+j , 0 , new QTableWidgetItem( verticalHeaderLabels[j] ) );
    }






    horizontalHeaderLabels.clear();

    horizontalHeaderLabels << "Знач" <<"a1" << "a2" << "a3"<< "a4" << "vsvr"<< "pin" << "pout"<< "u380" << "e1"<< "e2" << "e3"<< "e4" << "ec"<< "r0" << "r1"<< "r2" << "VSWR"<< "Pin"<< "Pout"<< "U380" ;


    for(int i = 0; i < 21; i++)
    {
       Table->setItem( 6 , i , new QTableWidgetItem( horizontalHeaderLabels[i] ) );
    }

    Table->setItem( 7 , 0 , new QTableWidgetItem( QString("Общ") ) );


    Table->resizeRowsToContents();
    Table->resizeColumnsToContents();

    Table->setColumnWidth(17, 45);
    Table->setColumnWidth(18, 45);
    Table->setColumnWidth(19, 45);
    Table->setColumnWidth(20, 45);

    Table->resize (750, 220); // initial size
    Table->setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Ignored);

    //connect(Table, SIGNAL(destroyed(QObject*)), showDataTableAction, SLOT(toggle()));

    //Table->show();

}

void PA_Shell::showDataTable(bool state)
{
    Q_UNUSED(state);
    Table->setVisible(true);
}

float PA_Shell::convert_Temperature_u16_to_float(quint16 temperature)
{
    int digit, minus = 0;
    float decimal;

    // Check if temperature is negative

    if (temperature & 0x8000)
    {
    // Two's complement, temperature is negative
        temperature = ~temperature + 1;
        minus = 1;
    }

    // Store temperature integer digits and decimal digits
    digit = temperature >> 4;
    digit |= ((temperature >> 8) & 0x7) << 4;

    //Store decimal digits
    decimal = temperature & 0x0F;
    decimal *= 0.0625f;

    // Check for negative part
    decimal = digit + decimal;
    if (minus)
    {
        decimal = 0 - decimal;
    }

    return decimal;
}

void PA_Shell::whiteAsk()
{
    ui->ask_label->setPixmap(QPixmap(":/icon/square1-white.png"));
}

void PA_Shell::redAnswer()
{
    ui->answer_label->setPixmap(QPixmap(":/icon/square1-red.png"));
}

void PA_Shell::whiteAnswer()
{
    ui->answer_label->setPixmap(QPixmap(":/icon/square1-white.png"));
}

void PA_Shell::ResetReceiver()
{
    if(port->isOpen())
    {
        queryTimer->stop();
        stateMachineChannel = 4;
        port->flush();
        portData.clear();
        queryTimer->start(QueryTime);
    }
}


void PA_Shell::noAnswer()
{

    if (  !(ui->Po_label->pixmap()->toImage() == QPixmap(":/icon/square1-white.png").toImage() )  )
   {
       if(port->isOpen())
        {
            port->close();
            port->open(QIODevice::ReadWrite);
        }

    }

   ResetReceiver();

    if (++noAnswerCounter >= 5)
    {
        if(doLog && (queryTimer->isActive())&&(5 == noAnswerCounter))
            logModulesLastData("------------Connection lost----------");

        noAnswerCounter = 10;  // не даем считать дальше

        ui->label_led_ch1Isen_4->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch1Isen_5->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch1Vg_1->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch1Vg_2->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch1Vg_3->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch1Vg_4->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch1Vg_5->setPixmap(QPixmap(":/icon/square1-white.png"));


        ui->label_led_ch2Isen_4->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch2Isen_5->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch2Vg_1->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch2Vg_2->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch2Vg_3->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch2Vg_4->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch2Vg_5->setPixmap(QPixmap(":/icon/square1-white.png"));


        ui->label_led_ch3Isen_4->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch3Isen_5->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch3Vg_1->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch3Vg_2->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch3Vg_3->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch3Vg_4->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch3Vg_5->setPixmap(QPixmap(":/icon/square1-white.png"));


        ui->label_led_ch4Isen_4->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch4Isen_5->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch4Vg_1->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch4Vg_2->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch4Vg_3->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch4Vg_4->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch4Vg_5->setPixmap(QPixmap(":/icon/square1-white.png"));

        ui->avail1_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->power1_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->U1_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->C1_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->t1_label->setPixmap(QPixmap(":/icon/square1-white.png"));

        ui->avail2_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->power2_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->U2_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->C2_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->t2_label->setPixmap(QPixmap(":/icon/square1-white.png"));

        ui->avail3_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->power3_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->U3_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->C3_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->t3_label->setPixmap(QPixmap(":/icon/square1-white.png"));

        ui->avail4_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->power4_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->U4_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->C4_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->t4_label->setPixmap(QPixmap(":/icon/square1-white.png"));

        ui->vswr_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->Pi_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->Po_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->cool_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->u380_label->setPixmap(QPixmap(":/icon/square1-white.png"));

        ui->Critical_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->lost_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->power0_label->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->present_label->setPixmap(QPixmap(":/icon/square1-white.png"));

        ui->label_led_ch1Block->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch2Block->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch3Block->setPixmap(QPixmap(":/icon/square1-white.png"));
        ui->label_led_ch4Block->setPixmap(QPixmap(":/icon/square1-white.png"));




    }

}

void PA_Shell::lightUp(QLabel * label, bool state)
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

void PA_Shell::onQueryTimer()
{
    //while(ProcessIncomingData());
    if(sendBuffer.isEmpty())requestStateMachine();
    outgoingQuery();
}


void PA_Shell::showAbout()
{
    QMessageBox::about(this, "О программе" , QString("Версия программы: ") + ProgramVersion );
}

void PA_Shell::requestStateMachine()
{
    stateMachineChannel++;

    if((stateMachineChannel >= 5)||(0 == stateMachineChannel))
    {
        stateMachineChannel = 0;
        if(serviceRequestAction->isChecked())
            getWholePaServiceDataCommand();
        else
            getWholePaDataCommand();
    }
    else // module
    {
        if(serviceRequestAction->isChecked())
            getModuleServiceDataCommand(stateMachineChannel);
        else
            getModuleDataCommand(stateMachineChannel);
    }
}


void PA_Shell::extendedView(bool state)
{
    ui->turnFirst_pushButton->setVisible(state);
    ui->turnSecond_pushButton->setVisible(state);
    ui->turnThird_pushButton->setVisible(state);
    ui->turnFourth_pushButton->setVisible(state);
    ui->label_Pout_1->setVisible(state);
    ui->label_Id_1->setVisible(state);
    ui->label_Pout_2->setVisible(state);
    ui->label_Id_2->setVisible(state);
    ui->label_Pout_3->setVisible(state);
    ui->label_Id_3->setVisible(state);
    ui->label_Pout_4->setVisible(state);
    ui->label_Id_4->setVisible(state);
    ui->label_16->setVisible(state);
    ui->label_17->setVisible(state);
}



void PA_Shell::writeSettings()
{
    QSettings settings(QString( QDir::currentPath() + "//settings.ini" )  , QSettings::IniFormat);

    settings.sync();

    settings.setValue("lastPort", port->portName());
    settings.setValue("baudRate", port->baudRate());
    settings.setValue("QueryTime", QueryTime);

    settings.setValue("doLog", QString::number(doLog));

    settings.setValue("logTimerInterval", (int)logTimerInterval/60000);

    settings.sync();

}

void PA_Shell::readSettings()
{

    if(QFile::exists(QDir::currentPath() + "//settings.ini"))
    {

        QSettings settings(QString( QDir::currentPath() + "//settings.ini" ), QSettings::IniFormat);

        if(settings.contains("lastPort"))
        {
            PortName = settings.value("lastPort", "COM1").toString();
            PortBaudRate = settings.value("baudRate",9600).toInt();
            port->setPortName(PortName);
            port->setBaudRate(PortBaudRate);

            QString type = "--";
            QStringRef sPortNum(&PortName,3,1);
            switch(MxGetInterface(sPortNum.toInt()))
            {
            case(MXU_RS485_4W):
            case(MXU_RS422):
                type = "4W";
                break;
            case(MXU_RS485_2W):
                type = "2W";
                break;
            }

            ui->comPortState_label->setText(QString( PortName  + "/" + QString::number(PortBaudRate))+ "/" + type );
        }

        if(settings.contains("QueryTime"))
        {
            QueryTime = settings.value("QueryTime", 200).toInt();
        }

    /*    if(settings.value("debugMode",0).toInt() == 1)
        {
            viewMenu->menuAction()->setVisible(1);
        }*/


        doLog = settings.value("doLog", 1).toInt();

        logTimerInterval = settings.value("logTimerInterval", 5).toInt();
        logTimerInterval = logTimerInterval * 60000;
        logTimer->setInterval(logTimerInterval);

    }

}

void PA_Shell::logTimerTask()
{
    if(doLog && (queryTimer->isActive())&&(5 > noAnswerCounter))
        logModulesLastData("------------Timer----------");
}

void PA_Shell::logModulesLastData(QString message)
{
Module_Data_Union_Type u;
Overall_Data_Union_Type uC;

    if(!logFile.isOpen())
        logFile.open(QIODevice::WriteOnly | QIODevice::Append |  QIODevice::Text);
    QTextStream out(&logFile);
    out <<  message << "\n";
    int i;
    for(i=1;i<5;i++)
    {
        u.Raw = ModuleFlags[i].Raw;
        out <<  QTime::currentTime().toString("hh:mm:ss") << ";"
             << "module:" << QString::number(i) << ";"
             << QString::number(dU[i],'f',1) << ";"
             << QString::number(dC[i],'f',1) << ";"
             << QString::number(dQ[i],'f',1) << ";"
             << QString::number(dT[i],'f',1) << ";"
             << QString::number(u.flags.I1) << ";"
             << QString::number(u.flags.I2) << ";"
             << QString::number(u.flags.I3) << ";"
             << QString::number(u.flags.I4) << ";"
             << QString::number(u.flags.I5) << ";"
             << QString::number(u.flags.U1) << ";"
             << QString::number(u.flags.U2) << ";"
             << QString::number(u.flags.U3) << ";"
             << QString::number(u.flags.U4) << ";"
             << QString::number(u.flags.U5) << ";"
             << QString::number(u.flags.Freq) << ";"
             << QString::number(u.flags.Volt) << ";"
             << QString::number(u.flags.Temp) << ";"
             << QString::number(u.flags.Enab) << ";"
             << QString::number(u.flags.Assert) << ";"
             << QString::number(u.flags.Respond) << ";"
             << "\n";
    }


        uC.Raw = CenterFlags.Raw;

        out <<  QTime::currentTime().toString("hh:mm:ss") <<";"
             << "center:" << ";"
             << QString::number(fVSWR,'f',1) << ";"
             << QString::number(fPin, 'f',1) << ";"
             << QString::number(fPout,'f',1) << ";"
             << QString::number(fU380,'f',1) << ";"
             << QString::number(uC.flags.air1) << ";"
             << QString::number(uC.flags.air2) << ";"
             << QString::number(uC.flags.air3) << ";"
             << QString::number(uC.flags.air4) << ";"
             << QString::number(uC.flags.vswr) << ";"
             << QString::number(uC.flags.pin) << ";"
             << QString::number(uC.flags.pout) << ";"
             << QString::number(uC.flags.u380) << ";"
             << QString::number(uC.flags.emergency1) << ";"
             << QString::number(uC.flags.emergency2) << ";"
             << QString::number(uC.flags.emergency3) << ";"
             << QString::number(uC.flags.emergency4) << ";"
             << QString::number(uC.flags.Critical) << ";"
             << QString::number(uC.flags.Pulse_Lost) << ";"
             << QString::number(uC.flags.Enab) << ";"
             << QString::number(uC.flags.Power_Present) << ";"
             << "\n";
        out.flush();
}



void PA_Shell::closeEvent (QCloseEvent *event)
{
    writeSettings();
    Table->deleteLater();

    event->accept();
}




