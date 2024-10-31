#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QDebug>
#include "httpserver.h"

const QString configPath = SERVER_CONFIG_FILE_PATH;


int main(int argc, char *argv[])
{
    Logger::initFromConfig(configPath);
    QCoreApplication app(argc, argv);

    HttpServer *httpServer = new HttpServer;
    httpServer->initFromConfig(configPath);
    httpServer->start();

    return app.exec();
}

