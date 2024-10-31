#include "logger.h"

// DEFAULT
LogLevel Logger::currentLogLevel = INFO;
QString Logger::logFilePath = "log_client.txt";

std::ofstream& Logger::getLogStream() {
    static std::ofstream logFile(logFilePath.toStdString());
    return logFile;
}

void Logger::setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

void Logger::setLogFilePath(const QString& path) {
    logFilePath = path;
    getLogStream().close();
    getLogStream().open(logFilePath.toStdString());
}

void Logger::initFromConfig(const QString& configFilePath) {
    if (checkConfig(configFilePath)) {
        Logger::log(ERROR, "Config file does not exist or empty, default values to logger were set");
        return;
    }

    const QSettings settings(configFilePath, QSettings::IniFormat);

    const QString logLevelStr = settings.value("Logging/LogLevel").toString();
    const QString logPath = settings.value("Logging/LogPath").toString();

    LogLevel level;
    if (logLevelStr == "DEBUG") {
        level = DEBUG;
    } else if (logLevelStr == "INFO") {
        level = INFO;
    } else if (logLevelStr == "WARNING") {
        level = WARNING;
    } else if (logLevelStr == "ERROR") {
        level = ERROR;
    } else {
        Logger::log(WARNING, "Invalid log level: " + logLevelStr + ", INFO logLevel selected");
        level = INFO;
    }

    setLogLevel(level);
    setLogFilePath(logPath);

    Logger::log(INFO, "Got logLevel and logPath from config to logger");
}

void Logger::log(LogLevel level, const QString& message) {
    if (level >= currentLogLevel) {
        qDebug() << levelToString(level) << ": " << message;

        std::ofstream& logFile = getLogStream();
        logFile << levelToString(level).toStdString() << ": " << message.toStdString() << std::endl;
    }
}

QString Logger::levelToString(LogLevel level) {
    switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}
