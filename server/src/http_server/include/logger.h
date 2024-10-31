#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <fstream>
#include <string>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void setLogLevel(LogLevel level);
    static void log(LogLevel level, const QString& message);
    static void setLogFilePath(const QString& path);
    static void initFromConfig(const QString& configFilePath);
private:
    static LogLevel currentLogLevel;
    static QString logFilePath;
    static QString levelToString(LogLevel level);
    static std::ofstream& getLogStream();
};




#endif // LOGGER_H

