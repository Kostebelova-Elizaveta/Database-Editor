#include "logindialog.h"
#include "ui_logindialog.h"
#include "logger.h"

LoginDialog::LoginDialog(MyNetworkClass *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    this->m = m;
    ui->setupUi(this);
    connect(m, &MyNetworkClass::requestAuthFinished, this, &LoginDialog::onRequestAuthFinished);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton_clicked()
{
    if (ui->idLineEdit->text() == "" or ui->passwordLineEdit->text() == "") {
        Logger::log(WARNING, "Error: Not all fields are filled");
        QMessageBox::warning(this, "Error", "Please fill in all fields!");
    }
    else {
        login_id = ui->idLineEdit->text().toInt();
        Logger::log(DEBUG, "Sending authorization request with ID=" + QString::number(login_id));
        m->authRequest(login_id, hashPassword(ui->passwordLineEdit->text()));
    }
}

void LoginDialog::onRequestAuthFinished(const QString &response) {
    Logger::log(DEBUG, "Received response from Authorization request");
    if (response == "You have been sucsessfuly authorized!") {
        Logger::log(INFO, "Authorization successful");
        QMessageBox::information(this, "Success", "You are logged in!");
        this->accept();
    }
    else {
        Logger::log(WARNING, "Authorization failed: Wrong ID or password");
        QMessageBox::warning(this, "Error", "Wrong ID or password!");
    }
}

int LoginDialog::getID() {
    return login_id;
}
