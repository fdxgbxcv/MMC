#include "modbus_server_setting.h"


void updateSerialPorts(Ui::MainWindow *ui)
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        ui->serialPortComboBox->addItem(info.portName());
    }
}

//更新第二个服务器的串口信息
void updateSerialPorts1(Ui::MainWindow *ui)
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        ui->serialPortComboBox1->addItem(info.portName());
    }
}

//连接至modbus服务器
void modbus_server_connect(Ui::MainWindow *ui, QWidget *parent)
{
    if (modbusMaster->state() == QModbusDevice::ConnectedState) {
        modbusMaster->disconnectDevice();
        delete modbusMaster;
        modbusMaster = new QModbusRtuSerialMaster(parent);
        ui->connectButton->setText("Connect");
    } else {
        modbusMaster->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->serialPortComboBox->currentText());
        modbusMaster->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        modbusMaster->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
        modbusMaster->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::EvenParity);
        modbusMaster->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, 1);

        if (modbusMaster->connectDevice()) {
            ui->connectButton->setText("Disconnect");
        } else {
            QString errorString = QString("Failed to connect to Modbus RTU server: %1").arg(modbusMaster->errorString());
            qDebug() << errorString;
            QMessageBox::critical(parent, "Connection Error", errorString); // 添加错误消息框
        }
    }
}

//连接至modbus服务器1
void modbus_server1_connect(Ui::MainWindow *ui, QWidget *parent)
{
    if (modbusMaster1->state() == QModbusDevice::ConnectedState) {
        modbusMaster1->disconnectDevice();
        delete modbusMaster1;
        modbusMaster1 = new QModbusRtuSerialMaster(parent);
        ui->connectButton_2->setText("Connect");
    } else {
        modbusMaster1->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->serialPortComboBox1->currentText());
        modbusMaster1->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        modbusMaster1->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
        modbusMaster1->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::NoParity);
        modbusMaster1->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, 1);

        if (modbusMaster1->connectDevice()) {
            ui->connectButton_2->setText("Disconnect");
        } else {
            QString errorString = QString("Failed to connect to Modbus RTU server: %1").arg(modbusMaster1->errorString());
            qDebug() << errorString;
            QMessageBox::critical(parent, "Connection Error", errorString); // 添加错误消息框
        }
    }
}

/***********************************************************串口读写*******************************************************************/
//读状态寄存器 需要readaddress与serverAddress与readnumber, 存于rir中
void readStatusRegister(int readaddress, int readnumber, int serverAddress, QObject *parent, const std::function<void(int)>& callback)
{
    if (modbusMaster->state() == QModbusDevice::ConnectedState) {
        QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters, readaddress, readnumber);

        QModbusReply *reply = modbusMaster->sendReadRequest(readUnit, serverAddress);

        if (reply) {
            QObject::connect(reply, &QModbusReply::finished, [parent, reply, callback] {
                if (reply->error() != QModbusDevice::NoError) {
                    qDebug() << "Read request error:" << reply->errorString();
                } else {
                    const QModbusDataUnit unit = reply->result();
                    rir = unit.value(0);
                    qDebug() << "Status register value:" << rir;
                    if (callback) {
                        callback(rir);
                    }
                }
                reply->deleteLater();
            });
        } else {
            qDebug() << "Failed to send read request:" << modbusMaster->errorString();
        }
    } else {
        qDebug() << "Not connected to a Modbus RTU server.";
    }
}

//读取保持寄存器 readaddress readnumber serverAddress 存于rhr中
void updateHoldingRegisters(int readaddress, int readnumber,int serverAddress, QObject *parent, const std::function<void(int)>& callback)
{
    if (modbusMaster->state() == QModbusDevice::ConnectedState) {
        QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, readaddress, readnumber);

        QModbusReply *reply = modbusMaster->sendReadRequest(readUnit, serverAddress);

        if (reply) {
            QObject::connect(reply, &QModbusReply::finished, [parent, reply,callback] {
                if (reply->error() != QModbusDevice::NoError) {
                    qDebug() << "Read request error:" << reply->errorString();
                } else {
                    const QModbusDataUnit unit = reply->result();
                    rhr = unit.value(0);
                    if (callback) {
                        callback(rhr);
                    }
                }
                reply->deleteLater();
            });
        } else {
            qDebug() << "Failed to send read request:" << modbusMaster->errorString();
        }
    }
}

//读取服务器1保持寄存器 readaddress readnumber serverAddress 存于rhr中
void updateHoldingRegisters1(int readaddress, int readnumber, int serverAddress, QObject *parent, const std::function<void(int)>& callback)
{
    if (modbusMaster1->state() == QModbusDevice::ConnectedState) {
        QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, readaddress, readnumber);

        QModbusReply *reply = modbusMaster1->sendReadRequest(readUnit, serverAddress);

        if (reply) {
            QObject::connect(reply, &QModbusReply::finished, [parent, reply, callback] {
                if (reply->error() != QModbusDevice::NoError) {
                    qDebug() << "Read request error:" << reply->errorString();
                } else {
                    const QModbusDataUnit unit = reply->result();
                    int rs1 = unit.value(0);
                    int rs2 = unit.value(1);

                    // 合并两个16位的值为一个32位的值
                    int combinedValue = (rs2 << 16) | rs1;

                    if (callback) {
                        callback(combinedValue);
                    }
                }
                reply->deleteLater();
            });
        } else {
            qDebug() << "Failed to send read request:" << modbusMaster1->errorString();
        }
    }
}

//写保持寄存器
void writeRegisters(int startAddress, int serverAddress, const QVector<quint16> &values,QObject* parent, const std::function<void(int)> callback)
{
//    qDebug() << "writeRegisters() called";
    if (modbusMaster->state() == QModbusDevice::ConnectedState) {
        QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, startAddress, values.size());

        for (int i = 0; i < values.size(); ++i) {
            whr=values[i];
            writeUnit.setValue(i, values[i]);
        }

        QModbusReply *reply = modbusMaster->sendWriteRequest(writeUnit, serverAddress);

        if (reply) {
            QObject::connect(reply, &QModbusReply::finished, [parent,reply,callback] {
                if (reply->error() != QModbusDevice::NoError) {
                    qDebug() << "Write request error:" << reply->errorString();
                } else {
                    qDebug() << "Write request successful.";
                }
                if (callback) {
                    callback(whr);
                }
                reply->deleteLater();  // 替换为此行
            });
        } else {
            qDebug() << "Failed to send write request:" << modbusMaster->errorString();
        }
    } else {
        qDebug() << "Not connected to a Modbus RTU server.";
    }
//    qDebug() << "writeRegisters() finished";
}
