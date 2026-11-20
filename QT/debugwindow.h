#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QDialog>
#include <QListWidget>
#include <QLabel>

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = nullptr);
    ~DebugWindow();


signals:
    void CustomCommand(QString sCommand, QString sParam1, QString sParam2);

public slots:

    void onIncomingMessage(QByteArray message);
    void onOutgoingMessage(QByteArray message);
    void onServiceMessage(QString message);
    void lightUp(QLabel * label, bool state);
    QLabel* getColorLabel(bool state);


private slots:
    void on_btnSend_clicked();

private:
    Ui::DebugWindow *ui;
};

#endif // DEBUGWINDOW_H
