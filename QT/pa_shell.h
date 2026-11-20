#ifndef PA_SHELL_H
#define PA_SHELL_H

#include <QMainWindow>
#include <QMenu>

#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSettings>
#include <QCloseEvent>
#include <QTableWidget>


#include <QAbstractNativeEventFilter>
#include <QAbstractEventDispatcher>
#include <qt_windows.h>
#include <dbt.h>
//#include "thermostat.h"
#include "comportsettings.h"
#include "ui_helpdialog.h"

namespace Ui {
class PA_Shell;
class HelpDialog;
}

class PA_Shell : public QMainWindow
{
    Q_OBJECT

public:
    explicit PA_Shell(QWidget *parent = nullptr);
    ~PA_Shell() override;


    QMenu *settingsMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
    QAction *showThermostatAction;
    QAction *startAction;
    QAction *showPortSettingsAction;
    QAction *showHelpAction;
    QAction *debugModeAction;
    QAction *aboutProgramAction;
    QAction *showAboutAction;
    QAction *extendedViewAction;
    QAction *serviceRequestAction;
    QAction *showDataTableAction;

    QByteArray portData;
    QList<QByteArray> sendBuffer;

    QTimer *queryTimer;
    int QueryTime;
    int PortBaudRate;
    QString PortName;

    QString getPortName(void){return PortName;}
    int getPortBaudRate(void){return PortBaudRate;}
    int getQueryTime(void){return QueryTime;}

    QTimer *askTimer;
    QTimer *answerTimer;
    QTimer *requestingStatusTimer;
    QTimer *outgoingTimer; //Отправка из буфера
    bool requestingStatus;
    QTimer *logTimer;

    int stateMachineChannel;
    int oldChan = 10;

    int noAnswerCounter;
    int doLog;
    int logTimerInterval;

    QTableWidget *Table;

    QFile logFile;

    quint16 lastModuleValue[6];
    quint16 lastModuleaskState[6];
    quint16 lastServiceValue[6];


signals:
    void incomingMessage(QByteArray message);
    void outgoingMessage(QByteArray message);
    void serviceMessage(QString);


public slots:

    double getAvgTemp(void);
    double getMaxTemp(void);
    void onDataAvailable();
    bool ProcessIncomingData();

    void createActions();
    void createMenus();

    void showPortSettings();
    void showHelp();


    void onStartQuery();
    void queryCycle();
    void outgoingQuery();
    void onPortSettingsChanged(QString name, QString baudrate, int queryTime, QString type);
    void uartDataProcess(QByteArray data);
    void onSendData(QByteArray byteArray);
    void turnPowerCommand();
    void turnPowerCommand(bool state);
    void turnSingleModuleCommand(int number);
    void turnFirst(){turnSingleModuleCommand(1);}
    void turnSecond(){turnSingleModuleCommand(2);}
    void turnThird(){turnSingleModuleCommand(3);}
    void turnFourth(){turnSingleModuleCommand(4);}
    void turnPowerCommandOn();
    void turnPowerCommandOff();
    void changeBaudRateCommand(int baudRate);
    void getModuleDataCommand(int channel);
    void getModuleServiceDataCommand(int moduleNumber);
    void getWholePaDataCommand();
    void onQueryTimer();
    void requestStateMachine();
    void showAbout();
    void showThermostat();
    void extendedView(bool state);
    void writeSettings();
    void readSettings();
    void SendCustomCommand(QString sCommand, QString sParam1, QString sParam2);


    void showDebugWindow();
    void createDataTable();
    void showDataTable(bool state);
    float convert_Temperature_u16_to_float(quint16 temperature);

    void whiteAsk();
    void redAnswer();
    void whiteAnswer();
    void noAnswer();
    void ResetReceiver();

    void lightUp(QLabel * label, bool state);

    void getWholePaServiceDataCommand();
    void FillFormWithModuleData(int chan);
    void logModulesLastData(QString message);
    void logTimerTask();

protected:
    void closeEvent (QCloseEvent *event) Q_DECL_OVERRIDE;
    void logWriteHeader();


    virtual void contextMenuEvent(QContextMenuEvent* pe) override
    {
        viewMenu->exec(pe->globalPos());
    }

private:
    //thermostat *tstat;

    Ui::PA_Shell *ui;
    QSerialPort *port;

    typedef struct
    {
        quint8 LSB;
        quint8 MSB;
    } Word_Bytes_Type;

    typedef union
    {
          qint16               Signed;
          quint16              Raw;
          Word_Bytes_Type      Byte;
    } Word_Bytes_Union_Type;

    typedef struct
    {
        quint16 LSW;
        quint16 MSW;
    } DWord_Words_Type;

    typedef union
    {
        DWord_Words_Type      DWord;
        quint32               Raw;
    } DWord_Words_Union_Type;

    typedef struct  {
        quint16 I1:1;
        quint16 I2:1;
        quint16 I3:1;
        quint16 I4:1;
        quint16 I5:1;
        quint16 U1:1;
        quint16 U2:1;
        quint16 U3:1;
        quint16 U4:1;
        quint16 U5:1;
        quint16 Freq:1;
        quint16 Volt:1;
        quint16 Temp:1;
        quint16 Enab:1;
        quint16 Assert:1;
        quint16 Respond:1;
    } Module_Data_Flags_Type;

    typedef union
    {
        Module_Data_Flags_Type flags;
        Word_Bytes_Type Byte;
        quint16 Raw;
    }Module_Data_Union_Type;

    typedef struct  {
        quint16 air1:1;
        quint16 air2:1;
        quint16 air3:1;
        quint16 air4:1;
        quint16 vswr:1;
        quint16 pin:1;
        quint16 pout:1;
        quint16 u380:1;
        quint16 emergency1:1;
        quint16 emergency2:1;
        quint16 emergency3:1;
        quint16 emergency4:1;
        quint16 Critical:1;
        quint16 Pulse_Lost:1;
        quint16 Enab:1;
        quint16 Power_Present:1;
    }Overall_Data_Flags_Type;

    typedef union
    {
        Overall_Data_Flags_Type flags;
        Word_Bytes_Type Byte;
        quint16 Raw;
    }Overall_Data_Union_Type;

    typedef enum
    {
        SEARCHSTART,
        WAITFULLSTART,
        WAITFULLDATA,
        COMMAND_POWER = 0x10,
        COMMAND_SETSPEED = 0x20,
        COMMAND_REQUEST_MODULE = 0x30,
        COMMAND_REQUEST_ALL = 0x40,
        COMMAND_SERVICE = 0x50,
        COMMAND_SERVICE_ALL = 0x60,
        WAIT_TX,
        IDLE
    } Protocol_State_Type;

    double dU[5], dC[5], dT[5], dQ[5], dP[5], dA[5];  // данные модулей
    Module_Data_Union_Type ModuleFlags[5];
    double fVSWR, fPin, fPout, fU380;
    Overall_Data_Union_Type CenterFlags;

    quint8 RequestChannelNumber=88;



};





class MyNativeEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
    Q_SIGNAL void deviceConnectionEvent();
public:
    explicit MyNativeEventFilter(QObject * parent = 0) : QObject(parent) {}
    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) Q_DECL_OVERRIDE
    {
        Q_UNUSED(eventType);
        Q_UNUSED(result);
        //QString type(eventType);
        MSG* msg = reinterpret_cast<MSG*>(message);
        int msgType = msg->message;

        if(msgType == WM_DEVICECHANGE)
        {
            switch(msg->wParam) {
            case DBT_DEVICEARRIVAL:
                MessageBox(NULL, __TEXT("System detected a new device"), __TEXT("System msg"), MB_OK);
                emit deviceConnectionEvent();
                break;
            case DBT_DEVICEREMOVECOMPLETE:
                MessageBox(NULL, __TEXT("Device is gone"), __TEXT("System msg"), MB_OK);
                emit deviceConnectionEvent();
                break;
            default:
                break;
            }
        }

        //MSG *msg = static_cast< MSG * >( message );
        // TODO: filter out or modify msg struct here
        // ...
        return false;
    }

};

#endif // PA_SHELL_H
