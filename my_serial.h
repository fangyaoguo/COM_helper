#ifndef MY_SERIAL_H
#define MY_SERIAL_H
#include "QtSerialPort/QSerialPortInfo"
 #include "QtSerialPort/QSerialPort"
#include "QObject"
class my_serial : public QObject
{
    Q_OBJECT
public:
    my_serial();
    void refresh_com();
    bool open();
    QSerialPortInfo info;
    QList<QSerialPortInfo> coms;
    void close();
    bool SetBaudRate(qint32 baudRate);
    bool SetDataBits(int a);
    bool SetStopBits(int a);
    bool SetParity(QString q);
    bool SetFlowControl();
    void SetPortName(QString &name);
    bool isOpen();
    int rx,tx;
    void transmit_Byte(QByteArray array);
    QSerialPort port;

};

#endif // MY_SERIAL_H
