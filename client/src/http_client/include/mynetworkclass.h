#ifndef MYNETWORKCLASS_H
#define MYNETWORKCLASS_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QObject>
#include "logger.h"
#include "helpers.h"

class MyNetworkClass : public QObject
{
    Q_OBJECT

public:
    MyNetworkClass();
    void getRequest();
    void getRequestFilter(QString filter, QString data);
    void deleteRequest(int id);
    void postRequest(QString password, QString name, QString surname, QString patronymic, QString year, int course, QString group_number, QImage picture);
    void authRequest(int id, QString password);
    void initFromConfig(const QString& configFilePath);

public slots:
    void onGetRequestFinished(QNetworkReply *reply);
    void onGetRequestFilterFinished(QNetworkReply *reply, QString filter);
    void onDeleteRequestFinished(QNetworkReply *reply);
    void onPostRequestFinished(QNetworkReply *reply);
    void onAuthRequestFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    QString host;
    QString port;

signals:
    void requestGetFinished(const QJsonArray& jsonArray);
    void requestGetFilterFinished(const QJsonArray& jsonArray);
    void requestDeleteFinished(const QString& response);
    void requestPostFinished(const QString& response);
    void requestAuthFinished(const QString& response);
};

#endif // MYNETWORKCLASS_H
