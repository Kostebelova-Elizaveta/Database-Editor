#ifndef HELPERS_H
#define HELPERS_H

#include <QString>
#include <QCryptographicHash>
#include <QByteArray>
#include <QDebug>
#include <QBuffer>
#include <QImage>
#include <QFileInfo>

QString hashPassword(const QString &password);
QString imageToBase64(const QImage& image);
bool checkConfig(const QString configFilePath);

#endif // HELPERS_H
