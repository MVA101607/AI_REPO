#ifndef COMPORTSETTINGS_H
#define COMPORTSETTINGS_H

#include <QDialog>
#include <QListWidget>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class ComPortSettings;
}

class ComPortSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ComPortSettings(QWidget *parent = nullptr);
    ~ComPortSettings();

signals:
    void portSettingsChanged(QString name = "COM1", QString baudrate = "9600", int queryTime = 200, QString type = "--");

public slots:
    void lookForPorts();
    void onPortSettingsChanged();
    void setPortSettings();
    void onListItemDClicked(QListWidgetItem * item);
    void Init(QString port, int baud, int delay);
    bool showInterfaceType(void);
    void setInterfaceType4w(void);
    void setInterfaceType2w(void);

private slots:
    void on_portListWidget_itemActivated(QListWidgetItem *item);

    void on_portName_comboBox_currentTextChanged(const QString &arg1);

    void on_portListWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::ComPortSettings *ui;
    QString type; // тип соединения - 2-проводной/4-проводной
};

#endif // COMPORTSETTINGS_H
