#include "helpers.h"

QString hashPassword(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

QString imageToBase64(const QImage& image) {
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "png");
    buffer.close();
    return arr.toBase64();
}

bool checkConfig(const QString configFilePath) {
  const QFileInfo configFileInfo(configFilePath);
  return !configFileInfo.exists() or configFileInfo.size() == 0;
}

