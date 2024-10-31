#include <gtest/gtest.h>
#include "httpserver.h"
#include <QFile>

QString getRandString(int l) {
    std::srand((unsigned int)time(NULL));
    int len = l;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return QString::fromStdString(tmp_s);
};

TEST(ServerCheck, DefaultSettingsCheck) {
    HttpServer *httpServer = new HttpServer;
    EXPECT_EQ(httpServer->getDbName(), "user");
    EXPECT_EQ(httpServer->getDbUserName(), "postgres");
    EXPECT_EQ(httpServer->getDbPassword(), "12345678");
    EXPECT_EQ(httpServer->getHost(), "localhost");
    EXPECT_EQ(httpServer->getPort(), "8080");
    EXPECT_EQ(httpServer->getAdminID(), "0");
    EXPECT_EQ(httpServer->getAdminPassword(), "a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3");

    delete httpServer;
}

TEST(ServerCheck, GetUpdatedDatabase) {
    HttpServer *httpServer = new HttpServer;
    QByteArray request = "GET /students HTTP/1.1\r\n"
                         "Host: localhost:8080\r\n\r\n";
    EXPECT_TRUE(httpServer->processRequest(request).contains("HTTP/1.1 200 OK\r\n"
                                                             "Content-Type: application/json\r\n\r\n"
                                                             "[\n    {"));
    delete httpServer;
}

TEST(ServerCheck, AddStudent) {
    HttpServer *httpServer = new HttpServer;
    QByteArray request = "POST /students/add HTTP/1.1\r\n"
                         "Host: localhost:8080\r\n"
                         "Content-Type: application/json\r\n"
                         "Accept: application/json\r\nContent-Length: 1056\r\n"
                         "Connection: Keep-Alive\r\n"
                         "Accept-Encoding: gzip, deflate\r\n"
                         "Accept-Language: en-US,*\r\n"
                         "User-Agent: Mozilla/5.0\r\n\r\n"
                         "{\"course\":6,"
                         "\"group_number\":\""
                         + getRandString(5).toUtf8() +
                         "\","
                         "\"name\":\""
                         + getRandString(4).toUtf8() +
                         "\","
                         "\"password\":\"6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b\"," // Hashed 123
                         "\"patronymic\":\""
                         + getRandString(14).toUtf8() +
                         "\","
                         "\"picture\":\""
                         + getRandString(872).toUtf8() +
                         "\","
                         "\"surname\":\""
                         + getRandString(11).toUtf8() +
                         "\","
                         "\"year\":\"2002-09-11\"}";
    qDebug() << request;
    ASSERT_EQ(httpServer->processRequest(request).toStdString(), "HTTP/1.1 201 Created\r\n"
                                                                 "Content-Type: text/plain\r\n\r\n"
                                                                 "Student successfully added!");
    delete httpServer;
}

TEST(ServerCheck, FilterRequestPresent) {
    HttpServer *httpServer = new HttpServer;
    QByteArray request = "GET /students/&filter=name&data=Liza HTTP/1.1\r\n"
                         "Host: localhost:8080\r\n\r\n";
    EXPECT_TRUE(httpServer->processRequest(request).contains("HTTP/1.1 200 OK\r\n"
                                                             "Content-Type: application/json\r\n\r\n"
                                                             "[\n    {"));
    delete httpServer;
}

TEST(ServerCheck, FilterRequestAbsent) {
    HttpServer *httpServer = new HttpServer;
    QByteArray request = "GET /students/&filter=password&data=Liza HTTP/1.1\r\n"
                         "Host: localhost:8080\r\n\r\n";

    ASSERT_EQ(httpServer->processRequest(request), "HTTP/1.1 200 OK\r\n"
                                                   "Content-Type: application/json\r\n\r\n"
                                                   "[\n]\n");
    delete httpServer;
}

TEST(ServerCheck, AutorizationSuccess) {
    HttpServer *httpServer = new HttpServer;
    QByteArray request = "GET /students/&"
                         "authorization&"
                         "id=0&"
                         "password=a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3"
                         " HTTP/1.1\r\n"
                         "User-Agent: PostmanRuntime/7.41.2\r\n"
                         "Accept: */*\r\n"
                         "Cache-Control: no-cache\r\n"
                         "Host: localhost:8080\r\n"
                         "Accept-Encoding: gzip, deflate, br\r\n"
                         "Connection: keep-alive\r\n\r\n";
    ASSERT_EQ(httpServer->processRequest(request), "HTTP/1.1 200 OK\r\n"
                                                   "Content-Type: text/plain\r\n\r\n"
                                                   "You have been sucsessfuly authorized!");
    delete httpServer;
}

TEST(ServerCheck, AuthorizationFailed) {
    HttpServer *httpServer = new HttpServer;
    QByteArray request = "GET /students/&"
                         "authorization&"
                         "id=12&"
                         "password=justwrong"
                         " HTTP/1.1\r\n"
                         "User-Agent: PostmanRuntime/7.41.2\r\n"
                         "Accept: */*\r\n"
                         "Cache-Control: no-cache\r\n"
                         "Host: localhost:8080\r\n"
                         "Accept-Encoding: gzip, deflate, br\r\n"
                         "Connection: keep-alive\r\n\r\n";
    ASSERT_EQ(httpServer->processRequest(request), "HTTP/1.1 500 Internal Server Error\r\n"
                                                   "Content-Type: text/plain\r\n\r\n"
                                                   "Wrong password or user's id!");
    delete httpServer;
}


TEST(ServerCheck, InvalidRequest) {
    HttpServer *httpServer = new HttpServer;
    QByteArray request = "GET /?something HTTP/1.1\r\nHost: localhost:8080\r\n"
                         "Content-Type: application/json\r\n"
                         "Accept: application/json\r\nContent-Length: 0\r\n\r\n";
    ASSERT_EQ(httpServer->processRequest(request).toStdString(), "HTTP/1.1 400 Bad Request\r\n"
                                                                 "Content-Type: text/plain\r\n\r\n"
                                                                 "Invalid request");
    delete httpServer;
}
