#ifndef MODBUS_SERVER_SETTING_H
#define MODBUS_SERVER_SETTING_H


#include "mainwindow.h"

QModbusRtuSerialMaster *modbusMaster;
QModbusRtuSerialMaster *modbusMaster1;

//using namespace Ui ;

//更新串口信息
void updateSerialPorts(Ui::MainWindow *ui);

//更新第二个服务器的串口信息
void updateSerialPorts1(Ui::MainWindow *ui);

//连接至modbus服务器
void modbus_server_connect(Ui::MainWindow *ui, QWidget *parent);

//连接至modbus服务器1
void modbus_server1_connect(Ui::MainWindow *ui, QWidget *parent);

/***********************************************************串口读写*******************************************************************/
//读状态寄存器 需要readaddress与serverAddress与readnumber, 存于rir中
void readStatusRegister(int readaddress, int readnumber, int serverAddress, QObject *parent, const std::function<void(int)>& callback);

//读取保持寄存器 readaddress readnumber serverAddress 存于rhr中
void updateHoldingRegisters(int readaddress, int readnumber,int serverAddress, QObject *parent, const std::function<void(int)>& callback);

//读取服务器1保持寄存器 readaddress readnumber serverAddress 存于rhr中
void updateHoldingRegisters1(int readaddress, int readnumber, int serverAddress, QObject *parent, const std::function<void(int)>& callback);

//写保持寄存器
void writeRegisters(int startAddress, int serverAddress, const QVector<quint16> &values,QObject* parent, const std::function<void(int)> callback);

#endif // MODBUS_SERVER_SETTING_H
