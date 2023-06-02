#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "modbus_server_setting.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    modbusMaster = new QModbusRtuSerialMaster(this);
    modbusMaster1 = new QModbusRtuSerialMaster(this);

    updateSerialPorts(ui);
    updateSerialPorts1(ui);
}

MainWindow::~MainWindow()
{
    delete ui;
    modbusMaster->disconnectDevice();
    modbusMaster1->disconnectDevice();
    delete modbusMaster;
    delete modbusMaster1;
}

