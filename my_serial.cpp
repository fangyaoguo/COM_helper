#include "my_serial.h"
#include "QDebug"
#include "QString"
#include "QByteArray"
my_serial::my_serial() {}

void my_serial::refresh_com()
{

    coms = info.availablePorts();
    qDebug() << "扫描串口 ：";
    for(auto i : coms)
       qDebug() << i.portName() << " ";
    qDebug() << '\n';
}

bool my_serial::open()
{
    rx = 0;
    tx = 0;
    qDebug() << port.portName() << " "
             << port.baudRate() << " "
             <<port.dataBits()<< " "
             <<port.flowControl() << " "
             <<port.stopBits() << " "
             <<port.parity() << '\n';
    return port.open(QIODeviceBase::ReadWrite);
}

void my_serial::close()
{
     port.close();
    rx = 0;
     tx = 0;
}

bool my_serial::SetBaudRate(qint32 baudRate)
{
    return port.setBaudRate(baudRate);
}

bool my_serial::SetDataBits(int a)
{
    switch(a){
        case 5:
            return port.setDataBits(QSerialPort::Data5);
            break;
        case 6:
            return port.setDataBits(QSerialPort::Data6);
            break;
        case 7:
            return port.setDataBits(QSerialPort::Data7);
            break;
        case 8:
            return port.setDataBits(QSerialPort::Data8);
            break;
    }
        return false;
}

bool my_serial::SetStopBits(QString stopBitsStr)
{
    if(stopBitsStr == "1")
        return port.setStopBits(QSerialPort::OneStop);
    else if(stopBitsStr == "1.5")
        return port.setStopBits(QSerialPort::OneAndHalfStop);
    else if(stopBitsStr == "2")
        return port.setStopBits(QSerialPort::TwoStop);
    else {
        qDebug() << "Invalid stop bits value:" << stopBitsStr;
        return false;
    }
}

bool my_serial::SetParity(QString q) //
{
    if(q == "None")
        return port.setParity(QSerialPort::NoParity);
    else if(q == "Odd")
        return port.setParity(QSerialPort::OddParity);
    else if(q == "Even")
        return port.setParity(QSerialPort::EvenParity);
    else if(q == "Mark")
       return port.setParity(QSerialPort::MarkParity);
    else if(q == "Space")
       return port.setParity(QSerialPort::SpaceParity);
    return false;
}

bool my_serial::SetFlowControl()
{
  return  port.setFlowControl(QSerialPort::NoFlowControl); //QSerialPort::SoftwareControl  QSerialPort::HardwareControl  QSerialPort::NoFlowControl
}

void my_serial::SetPortName(QString &name)
{
    port.setPortName(name);
}

bool my_serial::isOpen()
{
    return port.isOpen();
}

void my_serial::transmit_Byte(QByteArray array)
{

    if(!port.isOpen())
        return;
    tx += port.write(array);
}
