#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "pa_shell.h"
#include <QDialog>
#include <QListWidget>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "qcustomplot.h"

#define graphwidth  200
#define imax        10000
#define ibufsize    graphwidth

namespace Ui {
class thermostat;
}

class thermostat : public QDialog
{
    Q_OBJECT

public:
    explicit thermostat(PA_Shell *parent = nullptr);
    ~thermostat();

public slots:
    void lookForPorts();
    void writeSettings();
    void readSettings();
    void slider_process();
    void onPidTimer();
    void Draw();
    void onOneShotTimer();
    void UpdateVariables();
    void SendByte(char b);

signals:
    void getMaxTemp(int temp);

private slots:
    void on_horizontalSlider_sliderMoved(int position);

    void on_checkBox_PidEnable_stateChanged(int arg1);

    void on_pushButton_Connect_pressed();

    void on_pushButton_Stop_pressed();

    void on_pushButton_Start_pressed();

    void on_pushButton_50_pressed();

    void on_pushButton_100_pressed();
    void MyPortClose();

private:
    QString dbg_str;
    QCustomPlot *customPlot;
    PA_Shell *Parent;
    Ui::thermostat *ui;
    QTimer *pidTimer;
    QTimer *blinkTimer;
    QSerialPort *port;
    int Period;
    int try_count;
    char lastbyte;
    quint32 tickcount=graphwidth;
    int VentState=0, oldOut = -1, period;
    double Temp=0, Temp_ustavka, integral=0, oldError=0;
    double Kp, Ki, Kd, U;
    QVector<double> temp, xtime, y_out, ibuf;
};

#endif // THERMOSTAT_H
