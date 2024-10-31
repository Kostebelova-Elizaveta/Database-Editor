#ifndef DIALOGTOADD_H
#define DIALOGTOADD_H

#include <QWidget>
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QByteArray>
#include <QDebug>
#include "mynetworkclass.h"
#include "logger.h"
#include "helpers.h"

namespace Ui {
class DialogToAdd;
}

class DialogToAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogToAdd(MyNetworkClass *m = nullptr, QWidget *parent = nullptr);
    ~DialogToAdd();

signals:
    void updateTableSignal(QString response);

private slots:
    void on_addButton_clicked();
    void on_pickButton_clicked();
    void on_DialogToAdd_rejected();
    void onPostRequestFinished(const QString& response);

private:
    Ui::DialogToAdd *ui;
    MyNetworkClass *m;
    QByteArray arr;
    QImage image;
    QJsonArray reply;
};

#endif // DIALOGTOADD_H
