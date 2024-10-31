#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QString>
#include <QDebug>
#include <QBuffer>
#include <QPushButton>
#include <QTableWidgetItem>
#include "mynetworkclass.h"
#include "dialogtoadd.h"
#include "logindialog.h"
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(MyNetworkClass *m = nullptr, QWidget *parent = nullptr);
    ~MainWindow();
    void setID(int id);


private slots:
    void on_getButton_clicked();
    void on_lineEdit_returnPressed();
    void onDeleteButtonClicked(int row);
    void updateTable(QJsonArray jsonArray);
    void on_addButton_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_dateEdit_editingFinished();
    void onRequestGetFinished(const QJsonArray& jsonArray);
    void onRequestGetFilterFinished(const QJsonArray& jsonArray);
    void onRequestDeleteFinished(const QString& response);
    void onUpdateTableSignal(const QString& response);

public slots:
    void onLoginDialogAccepted(LoginDialog *logDiag);

private:
    Ui::MainWindow *ui;
    QStringList tableHeaders;
    QPushButton* deleteButton;
    MyNetworkClass *m;
    DialogToAdd *dialog;
    int login_user_id;

};
#endif // MAINWINDOW_H
