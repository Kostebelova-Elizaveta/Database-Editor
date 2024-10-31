#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logger.h"

MainWindow::MainWindow(MyNetworkClass *m, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->m = m;
    ui->setupUi(this);

//    GET REQUEST CONNECTION
    connect(m, &MyNetworkClass::requestGetFinished, this, &MainWindow::onRequestGetFinished);

//   FILTER GET REQUEST CONNECTION
    connect(m, &MyNetworkClass::requestGetFilterFinished, this, &MainWindow::onRequestGetFilterFinished);

//    DELETE REQUEST CONNECTION
    connect(m, &MyNetworkClass::requestDeleteFinished, this, &MainWindow::onRequestDeleteFinished);


    ui->dateEdit->hide();
    ui->lineEdit->show();

    ui->comboBox->addItem("ID");
    ui->comboBox->addItem("Name");
    ui->comboBox->addItem("Surname");
    ui->comboBox->addItem("Patronymic");
    ui->comboBox->addItem("Course");
    ui->comboBox->addItem("Year");
    ui->comboBox->addItem("Group number");

    ui->tableWidget->setColumnCount(9);
    tableHeaders<<"ID"<<"Name"<<"Surname"<<"Patronymic"<<"Year"<<"Course"<<"Group_number"<<"Picture"<<"Actions";
    ui->tableWidget->setHorizontalHeaderLabels(tableHeaders);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setIconSize(QSize(100, 100));
}


MainWindow::~MainWindow() {
    Logger::log(INFO, "MainWindow is deleted");
    delete ui;
}

void MainWindow::setID(int id) {
    m->getRequest(); // get request here is to load table when mainWindow is shown for the first time
    login_user_id = id;
    Logger::log(INFO, "Logged user's ID set, ID=" + QString::number(login_user_id));
}



void MainWindow::on_getButton_clicked() {
    ui->lineEdit->setText("");
    Logger::log(DEBUG, "Filters reset, sending GET request");
    m->getRequest();

}


void MainWindow::on_lineEdit_returnPressed() {
    QString filter, data;
    switch (ui->comboBox->currentIndex()) {
    case 0:
        filter = "student_id";
        break;
    case 1:
        filter = "name";
        break;
    case 2:
        filter = "surname";
        break;
    case 3:
        filter = "patronymic";
        break;
    case 4:
        filter = "course";
        break;
    case 5:
        filter = "year";
        break;
    case 6:
        filter = "group_number";
        break;
    default:
        break;
    }
    data = ui->lineEdit->text();
    Logger::log(DEBUG, "Sending GET request with filter=" + filter + ", data=" + data);
    m->getRequestFilter(filter, data);
}


void MainWindow::updateTable(QJsonArray jsonArray) {
    Logger::log(DEBUG, "Updating table with new data");
    ui->tableWidget->setRowCount(jsonArray.size());
    for (int i = 0; i < jsonArray.size(); ++i) {
        const QJsonObject jsonObject = jsonArray[i].toObject();

        const QString pictureBase64 = jsonObject.value("picture").toString();
        const QByteArray imageData = QByteArray::fromBase64(pictureBase64.toLatin1());
        QImage image;
        if (!image.loadFromData(imageData)) {
            Logger::log(ERROR, "libpng error: Read Error");
        }
        const QPixmap pixmap = QPixmap::fromImage(image);
        QTableWidgetItem *itemPicture = new QTableWidgetItem();
        itemPicture->setIcon(QIcon(pixmap));

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(jsonObject.value("student_id").toInt())));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(jsonObject.value("name").toString()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(jsonObject.value("surname").toString()));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(jsonObject.value("patronymic").toString()));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(jsonObject.value("year").toString()));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(jsonObject.value("course").toInt())));
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(jsonObject.value("group_number").toString()));
        ui->tableWidget->setItem(i, 7, itemPicture);
        deleteButton = new QPushButton("Delete", ui->tableWidget);
        ui->tableWidget->setCellWidget(i, ui->tableWidget->columnCount() - 1, deleteButton);
        // BUTTONS CONNECTION
        connect(deleteButton, &QPushButton::clicked, this, [this, i]() {
            onDeleteButtonClicked(i);
        });
        ui-> tableWidget->resizeRowToContents(i);
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}


void MainWindow::onDeleteButtonClicked(int row) {
    const int student_id = ui->tableWidget->item(row, 0)->text().toInt();
    Logger::log(DEBUG, "Delete button clicked for student with ID=" + QString::number(student_id));
    if (login_user_id == student_id) {
        QMessageBox::warning(this, "Error", "You can't delete yourself!");
        Logger::log(WARNING, "Warning: User tried to delete himself");
    }
    else {
        Logger::log(DEBUG, "Sending DELETE request for student with ID=" + QString::number(student_id));
        m->deleteRequest(student_id);
    }
}


void MainWindow::on_addButton_clicked() {
    Logger::log(DEBUG, "Add student button clicked");
    dialog = new DialogToAdd(m, this);

    // ADD CONNECTION
    connect(dialog, &DialogToAdd::updateTableSignal, this, &MainWindow::onUpdateTableSignal);

    dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowFullscreenButtonHint);
    dialog->exec();
}


void MainWindow::on_comboBox_currentIndexChanged(int index) {
    if (ui->comboBox->currentIndex() == 5) {
        Logger::log(DEBUG, "filter=" + ui->comboBox->itemText(index) + " has been selected, dateEdit shown");
        ui->dateEdit->show();
        ui->lineEdit->hide();
    }
    else {
        Logger::log(DEBUG, "filter=" + ui->comboBox->itemText(index) + " has been selected, lineEdit shown");
        ui->dateEdit->hide();
        ui->lineEdit->show();
    }
}


void MainWindow::on_dateEdit_editingFinished() {
    const auto data = ui->dateEdit->date().toString("yyyy-MM-dd");
    Logger::log(DEBUG, "Sending GET request with filter=year, data=" + data);
    m->getRequestFilter("year", data);
}


void MainWindow::onRequestGetFinished(const QJsonArray &jsonArray) {
    Logger::log(DEBUG, "Received response from GET request, updating table");
    updateTable(jsonArray);

}


void MainWindow::onRequestGetFilterFinished(const QJsonArray &jsonArray) {
    Logger::log(DEBUG, "Received response from GET request with filter, updating table");
        updateTable(jsonArray);
}


void MainWindow::onRequestDeleteFinished(const QString &response) {
    Logger::log(DEBUG, "Received response from DELETE request");
        if (response == "Student sucsessfuly deleted!") {
            Logger::log(INFO, "Student deleted successfully");
            this->m->getRequest();
        }
        else {
            Logger::log(WARNING, "Error deleting student: " + response);
        }
}


void MainWindow::onUpdateTableSignal(const QString &response) {
    Logger::log(DEBUG, "Received signal to update table, updating table");
    if (response == "Student successfully added!") {
        Logger::log(INFO, "Student added successfully");
        this->m->getRequest();
    }
    else {
        Logger::log(WARNING, "Error adding student: " + response);
    }
}


void MainWindow::onLoginDialogAccepted(LoginDialog *logDiag) {
    Logger::log(INFO, "MainWindow opened");
    this->setID(logDiag->getID());
    logDiag->close();
    this->show();
}
