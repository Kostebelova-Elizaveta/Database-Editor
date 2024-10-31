#include "mainwindow.h"
#include "logindialog.h"
#include "mynetworkclass.h"
#include "logger.h"
#include <QApplication>

const QString configPath = CLIENT_CONFIG_FILE_PATH;

int main(int argc, char *argv[])
{
//    Logger::setLogLevel(DEBUG);
    Logger::initFromConfig(configPath);
    QApplication a(argc, argv);
    MyNetworkClass *m = new MyNetworkClass();
    m->initFromConfig(configPath);
    LoginDialog logDiag(m, nullptr);
    MainWindow mainWindow(m, nullptr);

    logDiag.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    QObject::connect(&logDiag, &LoginDialog::accepted, [&logDiag, &mainWindow]() {
        mainWindow.onLoginDialogAccepted(&logDiag);
    });

    QObject::connect(&logDiag, &LoginDialog::rejected, [=]() {
        Logger::log(INFO, "Login dialog rejected, exiting application");
        QApplication::quit();
    });

    Logger::log(INFO, "Dalog to login opened");
    logDiag.show();
    return a.exec();
}
