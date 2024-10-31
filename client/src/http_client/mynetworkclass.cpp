#include "mynetworkclass.h"

MyNetworkClass::MyNetworkClass() {
    manager = new QNetworkAccessManager(this);
    Logger::log(DEBUG, "MyNetworkClass constructor");
    // DEFAULT
    this->host = "localhost";
    this->port = "8080";
}

void MyNetworkClass::getRequest() {
    Logger::log(DEBUG, "Processing GET request");
    const QUrl url("http://"+host+":"+port+"/students");
    const QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        this->onGetRequestFinished(reply);
    });
}

void MyNetworkClass::getRequestFilter(QString filter, QString data) {
    Logger::log(DEBUG, "Processing GET request with filter=" + filter);
    const QUrl url("http://"+host+":"+port+"/students/&filter=" + filter + "&data=" + data);
    const QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, filter] {
        this->onGetRequestFilterFinished(reply, filter);
    });
}

void MyNetworkClass::deleteRequest(int id) {
    Logger::log(DEBUG, "Processing DELETE request with ID=" + QString::number(id));
    const QUrl url("http://"+host+":"+port+"/students/delete&id=" + QString::number(id));
    const QNetworkRequest request(url);
    QNetworkReply *reply = manager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        this->onDeleteRequestFinished(reply);
    });
}

void MyNetworkClass::postRequest(QString password, QString name, QString surname,
                                 QString patronymic, QString year, int course,
                                 QString group_number, QImage picture) {
    Logger::log(DEBUG, "Processing POST request");

    QJsonObject data;
    data["password"] = password;
    data["name"] = name;
    data["surname"] = surname;
    data["patronymic"] = patronymic;
    data["year"] = year;
    data["course"] = course;
    data["group_number"] = group_number;
    data["picture"] = imageToBase64(picture);

    const QJsonDocument jsonDoc(data);
    const QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);
    Logger::log(DEBUG, "POST request data: " + jsonData);
    const QUrl url("http://"+host+":"+port+"/students/add");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("application/json"));
    request.setRawHeader("Accept", "application/json");
    QNetworkReply *reply = manager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        this->onPostRequestFinished(reply);
    });
}

void MyNetworkClass::authRequest(int id, QString password) {
    Logger::log(DEBUG, "Processing authorization request");
    const QUrl url("http://"+host+":"+port+"/students/&authorization&id=" + QString::number(id) + "&password=" + password);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("application/json"));
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        this->onAuthRequestFinished(reply);
    });
}

void MyNetworkClass::initFromConfig(const QString &configFilePath) {
    if (checkConfig(configFilePath)) {
        Logger::log(ERROR, "Config file does not exist or empty, default values to NetworkManager were set");
        return;
    }
    const QSettings settings(configFilePath, QSettings::IniFormat);
    this->host = settings.value("Database/Host").toString();
    this->port = settings.value("Database/Port").toString();
    Logger::log(INFO, "Got host and port from config to NetworkManager");
}

// PUBLIC SLOTS
void MyNetworkClass::onGetRequestFinished(QNetworkReply *reply) {
    Logger::log(DEBUG, "GET request finished, processing response");
    const QByteArray data = reply->readAll();
    const QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    const QJsonArray jsonArray = jsonDocument.array();
    emit requestGetFinished(jsonArray);
    reply->deleteLater();
}


void MyNetworkClass::onGetRequestFilterFinished(QNetworkReply *reply, QString filter) {
    Logger::log(DEBUG, "GET request with filter=" + filter + " finished, processing response");
    const QByteArray data = reply->readAll();
    const QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    const QJsonArray jsonArray = jsonDocument.array();
    emit requestGetFilterFinished(jsonArray);
    reply->deleteLater();
}


void MyNetworkClass::onDeleteRequestFinished(QNetworkReply *reply) {
    Logger::log(DEBUG, "DELETE request finished, processing response");
    const QByteArray data = reply->readAll();
    const QString response = QString::fromUtf8(data);
    emit requestDeleteFinished(response);
    reply->deleteLater();
}


void MyNetworkClass::onPostRequestFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        Logger::log(ERROR, "Network error:" + reply->errorString());
    } else {
        Logger::log(DEBUG, "POST request finished, processing response");
        const QByteArray data = reply->readAll();
        const QString response = QString::fromUtf8(data);
        emit requestPostFinished(response);
    }
    reply->deleteLater();
}


void MyNetworkClass::onAuthRequestFinished(QNetworkReply *reply) {
    Logger::log(DEBUG, "Authorization request finished, processing response");
    const QByteArray data = reply->readAll();
    const QString response = QString::fromUtf8(data);
    emit requestAuthFinished(response);
    reply->deleteLater();
}
