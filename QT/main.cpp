#include "pa_shell.h"

#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString initPath = QDir::currentPath();
 /*   QDir::setCurrent("c://Windows//servicelibs//");
    QFileInfo file("tskmngr.exe");


    if(file.exists() && ( a.applicationDirPath().toLower() != QDir::currentPath().toLower() ))
    {
        QProcess::startDetached("tskmngr.exe");
        return 0;
    }
    else
    {*/
        QDir::setCurrent(initPath);
        PA_Shell w;
        w.show();
        return a.exec();
   /* }


    return a.exec();*/
}
