#include "dialogtoadd.h"
#include "ui_dialogtoadd.h"

DialogToAdd::DialogToAdd(MyNetworkClass *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogToAdd) {
    Logger::log(INFO, "Dialog to add is opened");
    this->m = m;
    arr = nullptr;
    ui->setupUi(this);
    ui->pathToPictureEdit->setReadOnly(true);
    ui->courseEdit->setValidator(new QIntValidator);

    //POST REQUEST CONNECTION
    connect(m, &MyNetworkClass::requestPostFinished, this, &DialogToAdd::onPostRequestFinished);
}

DialogToAdd::~DialogToAdd()
{
    Logger::log(INFO, "Dialog to add is deleted");
    delete ui;
}

void DialogToAdd::onPostRequestFinished(const QString &response) {
    Logger::log(DEBUG, "Response from POST request received, sending signal and response to mainWindow");
    emit updateTableSignal(response);

    ui->passwordLineEdit->setText("");
    ui->nameEdit->setText("");
    ui->surnameEdit->setText("");
    ui->patronimycEdit->setText("");
    ui->courseEdit->setText("");
    ui->yearEdit->setDate(QDate(2000, 1, 1));
    ui->pathToPictureEdit->setText("");
    ui->groupNumberEdit->setText("");

    deleteLater();

}

void DialogToAdd::on_addButton_clicked() {
    if (ui->nameEdit->text() == "" or ui->surnameEdit->text() == ""
            or ui->patronimycEdit->text() == ""
            or ui->pathToPictureEdit->text() == "" or ui->passwordLineEdit->text() == ""
            or ui->courseEdit->text() == "" or ui->groupNumberEdit->text() == "") {
        Logger::log(WARNING, "Warning: Not all fields are filled");
        QMessageBox::warning(this, "Warning", "Please fill in all fields!");
        return;
    }
    else {
        Logger::log(DEBUG, "Sending POST request with data");
        m->postRequest(hashPassword(ui->passwordLineEdit->text()), ui->nameEdit->text(), ui->surnameEdit->text(), ui->patronimycEdit->text(), ui->yearEdit->date().toString("yyyy-MM-dd"), ui->courseEdit->text().toInt(), ui->groupNumberEdit->text(), image);
    }
}

void DialogToAdd::on_pickButton_clicked() {
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Choose image"),
                                                          "", tr("Images (*.png *.jpg *.jpeg)"));
    if (!fileName.isEmpty()) {
        QImage img(fileName);
        image = img;
        if (image.isNull()) {
            Logger::log(ERROR, "Error: Unable to load image");
            QMessageBox::warning(this, tr("Error"), tr("Unable to load image"));
            return;
        }
        Logger::log(DEBUG, "Image loaded: " + fileName);
        ui->pathToPictureEdit->setText(fileName);
    }
}

void DialogToAdd::on_DialogToAdd_rejected() {
    this->deleteLater();
}
