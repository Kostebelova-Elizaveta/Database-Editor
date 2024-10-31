#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "mynetworkclass.h"
#include <QMessageBox>
#include "helpers.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(MyNetworkClass *m = nullptr, QWidget *parent = nullptr);
    int getID();
    ~LoginDialog();

private slots:
    void on_loginButton_clicked();
    void onRequestAuthFinished(const QString& response);

private:
    Ui::LoginDialog *ui;
    MyNetworkClass *m;
    int login_id;
};

#endif // LOGINDIALOG_H
