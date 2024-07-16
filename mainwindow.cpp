#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QString>
#include <iostream>
#include <fstream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_pushButton_clicked);
    timer->start(1000);
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    // int k = interfaces.size();
    QString biba="";
    for (const QNetworkInterface &interface:interfaces){
        QString boba = interface.humanReadableName();
        ui->listWidget->addItem(boba);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

struct InterfaceStats
{
    unsigned long rx_packets;
    unsigned long tx_packets;
};

InterfaceStats getInterfaceStats (const QString& ifaceName){
    InterfaceStats stats = {0,0};
        QString rx_path="/sys/class/net/" + ifaceName + "/statistics/rx_packets";
        QString tx_path="/sys/class/net/" + ifaceName + "/statistics/tx_packets";
    try{
        ifstream rx_file(rx_path.toStdString());
        ifstream tx_file(tx_path.toStdString());
        if (!rx_file.is_open() || !tx_file.is_open()){
            throw runtime_error("Неудачная попытка открытия файла");
        }
        rx_file>>stats.rx_packets;
        tx_file>>stats.tx_packets;
        rx_file.close();
        tx_file.close();
    }
        catch(...){
            stats.rx_packets=0;
            stats.tx_packets=0;
        }

        return stats;
}

struct DuplexStats
{
    std::string duplex;
    unsigned long speed;
};

DuplexStats getDuplexStats (const QString& ifaceName){
    DuplexStats stats = {"",0};
    QString duplex_path = "/sys/class/net/" + ifaceName + "/duplex";
    QString speed_path = "/sys/class/net/" + ifaceName + "/speed";
    try{
        ifstream duplex_file(duplex_path.toStdString());
        ifstream speed_file(speed_path.toStdString());
        if (!duplex_file.is_open() || !speed_file.is_open()){
            throw runtime_error("Неудачная попытка открытия файла");
        }
        duplex_file>>stats.duplex;
        speed_file>>stats.speed;
    }
    catch(...){
        stats.duplex="Не найдено";
        stats.speed=0;
    }

    return stats;
}

void MainWindow::give_me_my_ip(int a){
    QString ifaceName = (ui->listWidget->item(a))->text();
    QNetworkInterface iface = QNetworkInterface::interfaceFromName(ifaceName);

    const QList<QNetworkAddressEntry> entries = iface.addressEntries();
    foreach (const QNetworkAddressEntry &entry, entries){
        if (entry.ip().protocol()==QAbstractSocket::IPv4Protocol){
            ui->textEdit->setText(entry.ip().toString());
        }
        ui->textEdit_2->setText(entry.netmask().toString());
    }
    ui->listWidget->show();
}

void MainWindow::on_pushButton_clicked()
{
    int a = (ui->listWidget->currentRow());
    if (a==-1) a=0;
    give_me_my_ip(a);

        InterfaceStats stats = getInterfaceStats((ui->listWidget->item(a))->text());
        ui->textEdit_5->setText(QString::number(stats.rx_packets));
        ui->textEdit_11->setText(QString::number(stats.tx_packets));

    DuplexStats stat=getDuplexStats((ui->listWidget->item(a))->text());
        ui->textEdit_3->setText(QString::number(stat.speed));
        ui->textEdit_4->setText(QString::fromStdString(stat.duplex));
}
