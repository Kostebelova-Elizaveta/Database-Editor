#include "httpserver.h"

HttpServer::HttpServer(QObject *parent) : QObject(parent)
{
    //DEFAULT
    this->host = "localhost";
    this->port = "8080";
    this->DbName = "user";
    this->DbPassword = "12345678";
    this->DbUserName = "postgres";
    this->adminID = "0";
    this->adminPassword = "a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3";
    Logger::log(DEBUG, "HttpServer constructor");

    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &HttpServer::onNewConnection);

    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(host);
    db.setDatabaseName(DbName);
    db.setPassword(DbPassword);
    db.setUserName(DbUserName);
    if (!db.open()) {
        Logger::log(ERROR, "Error connecting to database: " + db.lastError().text());
    } else {
        Logger::log(INFO, "Database connection established");
    }
}

HttpServer::~HttpServer()
{
    db.close();
}

void HttpServer::start()
{
    if (!server->listen(QHostAddress::Any, port.toUShort())) {
        Logger::log(ERROR, "Server failed to listen on port " + port + ": " + server->errorString());
        return;
    }

    Logger::log(INFO, "Server started on port " + port);
}

void HttpServer::onNewConnection()
{
    Logger::log(INFO, "New client connected");
    QTcpSocket *clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &HttpServer::onDataReceived, Qt::QueuedConnection);
    connect(clientSocket, &QTcpSocket::disconnected, this, &HttpServer::onClientDisconnected, Qt::QueuedConnection);
}

void HttpServer::onDataReceived()
{
    QTcpSocket *clientSocket = static_cast<QTcpSocket*>(sender());
    QByteArray data;
    data = clientSocket->readAll();

    Logger::log(DEBUG, "Received data: " + data);

    //        QString request = QString::fromLatin1(data);
    //        qDebug() << request;
    QString response = processRequest(data);

    clientSocket->write(response.toLatin1());
    clientSocket->disconnectFromHost();
}

void HttpServer::onClientDisconnected()
{
    Logger::log(INFO, "Client disconnected");
    QTcpSocket *clientSocket = static_cast<QTcpSocket*>(sender());
    clientSocket->deleteLater();
}

QString HttpServer::updatedDatabase() {
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare("SELECT * FROM student");
    if (sqlQuery.exec()) {
        QJsonArray studentsArray;
        while (sqlQuery.next()) {
            QJsonObject studentObject;
            studentObject["student_id"] = sqlQuery.value(0).toInt();
            studentObject["password"] = sqlQuery.value(1).toString().toHtmlEscaped();
            studentObject["name"] = sqlQuery.value(2).toString().toHtmlEscaped();
            studentObject["surname"] = sqlQuery.value(3).toString().toHtmlEscaped();
            studentObject["patronymic"] = sqlQuery.value(4).toString().toHtmlEscaped();
            studentObject["year"] = sqlQuery.value(5).toString().toHtmlEscaped();
            studentObject["course"] =  sqlQuery.value(6).toInt();
            studentObject["group_number"] = sqlQuery.value(7).toString().toHtmlEscaped();
            studentObject["picture"] = QString::fromLatin1(sqlQuery.value(8).toByteArray().toBase64());

            studentsArray.append(studentObject);
        }

        const QJsonDocument jsonDocument(studentsArray);
        const QByteArray jsonData = jsonDocument.toJson();

        QString response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
        Logger::log(INFO, "Request GET sucsessfuly completed");
        response += jsonData;
        return response;
    }
    else {
        Logger::log(ERROR, "Error executing query while returning updated database: " + sqlQuery.lastError().text());
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\n"
               "Error executing query while returning all database: " + sqlQuery.lastError().text();
    }
}

void HttpServer::initFromConfig(const QString &configFilePath)
{
    QFileInfo configFileInfo(configFilePath);
    if (!configFileInfo.exists() or configFileInfo.size() == 0) {
        Logger::log(ERROR, "Config file does not exist or empty, default values to server were set");
        return;
    }

    const QSettings settings(configFilePath, QSettings::IniFormat);

    this->host = settings.value("Database/Host").toString();
    this->port = settings.value("Database/Port").toString();
    this->DbName = settings.value("Database/DbName").toString();
    this->DbPassword = settings.value("Database/DbPassword").toString();
    this->DbUserName = settings.value("Database/DbUserName").toString();
    this->adminID = settings.value("Admin/Login").toString();
    this->adminPassword = settings.value("Admin/Password").toString();

    Logger::log(INFO, "Got host, port, adminID, adminPassword, "
                      "DbName, DbPassword and DbUserName from config to server");
}

QString HttpServer::processRequest(const QByteArray &request) {
    QUrlQuery query(request);
    QSqlQuery sqlQuery(db);
    // GET REQUESTS
    if (request.contains("GET") and request.contains("students")) {
        if (request.contains("filter=")) {
            Logger::log(INFO, "Request GET with filter received");
            auto listOfArgs = QString(request).split(" ").at(1).split("&");
            QString filter = listOfArgs.at(1).split("=").at(1);
            QString data = listOfArgs.at(2).split("=").at(1);
            Logger::log(DEBUG, "Filter: " + filter);
            Logger::log(DEBUG, "The value to be filtered by: " + data);
            if (filter == "student_id" or filter == "course") {
                sqlQuery.prepare("SELECT * FROM student WHERE " + filter + "=:data");
                sqlQuery.bindValue(":data", data.toInt());
            }
            else {
                if (filter == "year") {
                    sqlQuery.prepare("SELECT * FROM student WHERE " + filter + "=:data");
                    sqlQuery.bindValue(":data", data);
                }
                else {
                    sqlQuery.prepare("SELECT * FROM student WHERE " + filter + " LIKE :data");
                    sqlQuery.bindValue(":data", "%" + data + "%");
                }
            }

            if (sqlQuery.exec()) {
                QJsonArray studentsArray;
                while (sqlQuery.next()) {
                    QJsonObject studentObject;
                    studentObject["student_id"] = sqlQuery.value(0).toInt();
                    studentObject["password"] = sqlQuery.value(1).toString().toHtmlEscaped();
                    studentObject["name"] = sqlQuery.value(2).toString().toHtmlEscaped();
                    studentObject["surname"] = sqlQuery.value(3).toString().toHtmlEscaped();
                    studentObject["patronymic"] = sqlQuery.value(4).toString().toHtmlEscaped();
                    studentObject["year"] = sqlQuery.value(5).toString().toHtmlEscaped();
                    studentObject["course"] =  sqlQuery.value(6).toInt();
                    studentObject["group_number"] = sqlQuery.value(7).toString().toHtmlEscaped();
                    studentObject["picture"] = QString::fromLatin1(sqlQuery.value(8).toByteArray().toBase64());

                    studentsArray.append(studentObject);
                }

                const QJsonDocument jsonDocument(studentsArray);
                const QByteArray jsonData = jsonDocument.toJson();

                Logger::log(INFO, "Request GET with filter sucsessfuly completed");

                QString response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
                response += jsonData;
                return response;
            } else {
                Logger::log(ERROR, "Error executing query while completing request GET with filter: " + sqlQuery.lastError().text());
                return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\n"
                       "Error executing query while completing request GET with filter: " + sqlQuery.lastError().text();
            }
        }
        else {
            if (request.contains("authorization")) {
                Logger::log(INFO, "Request GET authorization received");
                auto listOfArgs = QString(request).split(" ").at(1).split("&");
                const QString id = listOfArgs.at(2).split("=").at(1);
                const QString password = listOfArgs.at(3).split("=").at(1);
                Logger::log(DEBUG, "ID to log in: " + id);
                Logger::log(DEBUG, "Password to log in: " + password);
                sqlQuery.prepare("SELECT * FROM student WHERE student_id=:id AND password=:password");
                sqlQuery.bindValue(":id", id.toInt());
                sqlQuery.bindValue(":password", password);
                if (sqlQuery.exec()) {
                    if (sqlQuery.next() or ((id == this->adminID) and (password == this->adminPassword))) {
                        Logger::log(INFO, "Request GET authorization sucsessfuly completed, user logged in");
                        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
                               "You have been sucsessfuly authorized!";
                    }
                    else {
                        Logger::log(WARNING, "Request GET authorization failed, wrong password or user's id");
                        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\n"
                               "Wrong password or user's id!";
                    }

                }
                else {
                    Logger::log(ERROR, "Error executing query while request GET authorization: " + sqlQuery.lastError().text());
                    return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\n"
                           "Error executing query while request GET authorization: " + sqlQuery.lastError().text();
                }
            }
            else {
                Logger::log(INFO, "Request GET received");
                return updatedDatabase();
            }
        }

        // POST REQUESTS
    } else if (request.contains("POST") and request.contains("students/add")) {
        Logger::log(INFO, "Request POST received");
        const int headerEnd = request.indexOf("\r\n\r\n");
        if (headerEnd == -1) {
            Logger::log(ERROR, "Error parsing request POST");
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\n"
                   "Error parsing request";
        }

        const QByteArray headers = request.left(headerEnd + 4);
        const QByteArray body = request.mid(headerEnd + 4);

        Logger::log(DEBUG, "Headers: " + headers);
        Logger::log(DEBUG, "Body: " + body);

        const QJsonObject jsonData = QJsonDocument::fromJson(body).object();

        const QString password = jsonData["password"].toString();
        const QString name = jsonData["name"].toString();
        const QString surname = jsonData["surname"].toString();
        const QString patronymic = jsonData["patronymic"].toString();
        const QString year = jsonData["year"].toString();
        const int course = jsonData["course"].toInt();
        const QString groupNumber = jsonData["group_number"].toString();
        const QByteArray picture = QByteArray::fromBase64(jsonData["picture"].toString().toLatin1());

        Logger::log(DEBUG, "Parsed JSON data from request's body: "
                          "password=" + password + " name=" + name +
                          " surname=" + surname +
                          " patronymic=" + patronymic +
                          " year=" + year + " course=" + QString::number(course) +
                          " groupNumber=" + groupNumber);


        sqlQuery.prepare("INSERT INTO student "
                         "(password, name, surname, "
                         "patronymic, year, course, "
                         "group_number, picture) "
                         "VALUES (:password, :name, :surname, "
                         ":patronymic, :year, :course, "
                         ":group_number, :picture)");

        sqlQuery.bindValue(":password", password);
        sqlQuery.bindValue(":name", name);
        sqlQuery.bindValue(":surname", surname);
        sqlQuery.bindValue(":patronymic", patronymic);
        sqlQuery.bindValue(":year", year);
        sqlQuery.bindValue(":course", course);
        sqlQuery.bindValue(":group_number", groupNumber);
        sqlQuery.bindValue(":picture", picture);

        if (sqlQuery.exec()) {
            Logger::log(INFO, "Request POST sucsessfuly completed, student successfully added");
            return "HTTP/1.1 201 Created\r\nContent-Type: text/plain\r\n\r\n"
                   "Student successfully added!";

        } else {
            Logger::log(ERROR, "Error executing query while completing request POST: " + sqlQuery.lastError().text());
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\n"
                   "Error executing query while completing request POST: " + sqlQuery.lastError().text();
        }

        // DELETE REQUESTS
    } else if (request.contains("DELETE") and request.contains("students/delete")) {
        Logger::log(INFO, "Request DELETE received");
        const auto listOfArgs = QString(request).split(" ").at(1).split("&");
        const QString data = listOfArgs.at(1).split("=").at(1);
        Logger::log(DEBUG, "ID of the user to be deleted: " + data);
        sqlQuery.prepare("DELETE FROM student WHERE student_id = :data");
        sqlQuery.bindValue(":data", data.toInt());
        if (sqlQuery.exec()) {
            Logger::log(INFO, "Request DELETE sucsessfuly completed, student sucsessfuly deleted");
            return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
                   "Student sucsessfuly deleted!";
        }
        else {
            Logger::log(ERROR, "Error executing query while completing request DELETE: " + sqlQuery.lastError().text());
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\n"
                   "Error executing query while completing request DELETE: " + sqlQuery.lastError().text();
        }

    }
    else {
        Logger::log(ERROR, "Invalid request");
        return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\n"
               "Invalid request";
    }
}

// Getters for tests
QString HttpServer::getDbName() {
    return DbName;
}

QString HttpServer::getDbPassword() {
    return DbPassword;
}

QString HttpServer::getDbUserName() {
    return DbUserName;
}

QString HttpServer::getHost() {
    return host;
}

QString HttpServer::getPort() {
    return port;
}

QString HttpServer::getAdminID() {
    return adminID;
}

QString HttpServer::getAdminPassword() {
    return adminPassword;
}
