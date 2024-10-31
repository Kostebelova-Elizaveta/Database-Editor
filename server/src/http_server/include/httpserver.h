#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QDebug>
#include <QSqlDatabase>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include "logger.h"


class HttpServer : public QObject
{
    Q_OBJECT

public:
    explicit HttpServer(QObject *parent = nullptr);
    ~HttpServer();
    QString processRequest(const QByteArray &request);
    void start();

    void initFromConfig(const QString& configFilePath);
    
    QString getDbName();

    QString getDbPassword();

    QString getDbUserName();

    QString getHost();

    QString getPort();

    QString getAdminID();

    QString getAdminPassword();

private slots:
    void onNewConnection();

    void onDataReceived();

    void onClientDisconnected();
private:

    QString updatedDatabase();

    QSqlDatabase db;
    QTcpServer *server;
    QString host;
    QString port;
    QString DbName; //("user");
    QString DbPassword; //("12345678");
    QString DbUserName; //("postgres");
    QString adminID;
    QString adminPassword;
};



#endif // HTTPSERVER_H

