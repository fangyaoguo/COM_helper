#include "widget.h"
#include "./ui_widget.h"
#include "QDebug"
#include "QFile"
#include "QString"
#include "QFileDialog"
#include "QTextStream"
#include "QMessageBox"
#include "QByteArray"
#include "QTimer"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QRect mainWindow = {1024,300,600,600};
    this->setMinimumHeight(600);
    this->setGeometry(mainWindow);
    set_pos();
    emit ui->refresh_serial->clicked();
    QObject::connect(ui->pushButton_clear,&QPushButton::clicked,ui->textEdit_receive,[=](){
        ui->textEdit_receive->clear();
    });
    QObject::connect(ui->pushButton_clear_send,&QPushButton::clicked,ui->textEdit_transmit,[=](){
        ui->textEdit_transmit->clear();
    });
    QObject::connect(&serial.port,&QSerialPort::readyRead,this,&Widget::handleSerialData);
    ui->spinBox->setMinimum(10);
    ui->spinBox->setMaximum(5000);
    ui->spinBox->setSingleStep(10);
    autoSendTimer = new QTimer(this);
    connect(autoSendTimer, &QTimer::timeout,
            this, &Widget::on_pushButton_transmit_clicked);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::set_pos()
{

    int window_x = this->width();
    int window_y = this->height();
    qDebug() << "now height is " << window_y << " width is "<<window_x <<"\n";
    QRect bottom;
    bottom.setRect(0,window_y-35,window_x,40);
    ui->frame_Bottom->setGeometry(bottom);
    QRect serial = {5,0,150,250};
    ui->frame_serial->setGeometry(serial);
    QRect recv ={5,255,150,100};
    ui->frame_receive->setGeometry(recv);
    QRect sender = {5,360,150,200};
    ui->frame_sender->setGeometry(sender);
    QRect recv_t = {155,0,window_x -160,(window_y-45) / 5 * 3};
    ui->frame_receive_text->setGeometry(recv_t);
    QRect trans_t = {155,(window_y - 45)/ 5 * 3 + 5,window_x - 160,(window_y - 45) / 5 * 2};
    ui->frame_transmit_text->setGeometry(trans_t);

}

void Widget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "the window has been resized";
    set_pos();
}

void Widget::handleSerialData()
{
    // 1. 读取所有可用数据
    QByteArray receivedData = serial.port.readAll();
    serial.rx += receivedData.size();
    ui->label_RX->setText("RX: " + QString::number(serial.rx));
    // 2. 转换为可显示格式（根据实际编码需求选择）
    QString displayText;

    // 方案1：直接显示原始ASCII（适合英文）
    // displayText = QString::fromLatin1(receivedData);

    // 方案2：UTF-8解码（推荐，支持中文）
    if(!ui->checkBox_HEX->isChecked())
    displayText = QString::fromUtf8(receivedData);

    // 方案3：十六进制显示
    else {
        QByteArray hexData = receivedData.toHex(' ').toUpper();
        displayText = QString::fromLatin1(hexData);
    }

    // 3. 追加到文本编辑框
    ui->textEdit_receive->moveCursor(QTextCursor::End);
    QString timestamp = QDateTime::currentDateTime()
                            .toString("[hh:mm:ss.zzz] ");
    if(ui->checkBox_timeTick->isChecked()){
        ui->textEdit_receive->insertPlainText(timestamp + displayText);
        ui->textEdit_receive->insertPlainText("\n");
    }
    else ui->textEdit_receive->insertPlainText(displayText);



    // 4. （可选）自动滚动到底部
    ui->textEdit_receive->ensureCursorVisible();
}


void Widget::on_pushButton_save_clicked()
{
    QFile file;
    QString filename;
    filename = QFileDialog::getSaveFileName(this,tr("Save"),"D:/",tr("Text (*.txt)"));
        if(filename.size() == 0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("未选择文件名"));
            return;
    }
    file.setFileName(filename);
    if(!file.open(QFile::NewOnly |QFile::WriteOnly))
    {
        QMessageBox::warning(this,tr("report"),tr("文件打开错误"));
         return;
    }

    QTextStream out(&file);
    out<<ui->textEdit_receive->toPlainText();
    file.close();
    QMessageBox::information(this,tr("notice"),tr("文件保存成功"));
}


void Widget::on_pushButton_read_clicked()
{
    QFile file;
    QString filename;
    filename = QFileDialog::getOpenFileName(this,tr("open"),tr("D:/"),tr("Text (*.txt *.ini)"));
    file.setFileName(filename);
    if(filename.size() == 0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("未选择文件名"));
        return;
    }
    if(!file.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this,tr("report"),tr("文件打开错误"));
        return;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString tmp;
        tmp = in.readLine();
        ui->textEdit_transmit->insertPlainText(tmp);
    }
}


void Widget::on_Serial_button_clicked()
{
    if(ui->serial_select->count() == 0){
        QMessageBox::warning(this,"warning","无串口！");
        return;}

    bool is_open = serial.isOpen();// 检测串口状态。
    if(!is_open)
    {
        QString portname = ui->serial_select->currentText();
        serial.SetPortName(portname);
        serial.SetBaudRate(ui->baud_select->currentText().toLong());
        serial.SetDataBits(ui->Data_bit_sclect->currentText().toInt());
        serial.SetFlowControl();
        serial.SetStopBits(ui->Stop_bit_select->currentText());
        serial.SetParity(ui->Parity_select->currentText());
        if(serial.open()){
         ui->Serial_button->setText(tr("关闭串口"));

            qDebug() << "串口打开";}
        else QMessageBox::warning(this,"warning","串口打开失败！");
    }
    else{
        serial.close();
        ui->Serial_button->setText(tr("打开串口"));
        qDebug()<< "串口关闭";
    }
    ui->label_TX->setText("TX: " + QString::number(serial.tx));
    ui->label_RX->setText("RX: " + QString::number(serial.rx));
    is_open = serial.isOpen();
    ui->serial_select->setEnabled(!is_open);
    ui->Parity_select->setEnabled(!is_open);
    ui->baud_select->setEnabled(!is_open);
    ui->Stop_bit_select->setEnabled(!is_open);
    ui->Data_bit_sclect->setEnabled(!is_open);

}


void Widget::on_refresh_serial_clicked()
{
    serial.refresh_com();
    ui->serial_select->clear();
    for(auto i : serial.coms)
    {
        ui->serial_select->addItem(i.portName());
    }
}


void Widget::on_pushButton_transmit_clicked()
{
    if(!serial.isOpen())return;
    QString text = ui->textEdit_transmit->toPlainText();
    if(ui->checkBox_THEX->isChecked())
    {
        //检测输入是否符合规范
        // 移除所有空格，方便用户输入（创建新字符串避免修改原始文本）
        QString cleanText = QString(text).remove(' ').remove('\n').remove('\r').remove('\t');
        QByteArray tmp = cleanText.toLatin1();
        if(tmp.size()%2 != 0){
            QMessageBox::information(this,"warn","输入不合法！十六进制字符数必须为偶数");return;}
        for(char c : tmp)
        {
            if(!std::isxdigit(c))
            {
                QMessageBox::information(this,"warn","输入不合法！只能包含0-9和A-F字符");return;
            }
        }
        QByteArray send = QByteArray::fromHex(tmp);
        serial.transmit_Byte(send);
        return;
    }
    if(ui->checkBox_endl->isChecked())text += "\r\n";  // 追加回车换行符
     QByteArray byteArray = text.toUtf8();
    serial.transmit_Byte(byteArray);
    ui->label_TX->setText("TX: " + QString::number(serial.tx));

}



void Widget::on_checkBox_transmit_checkStateChanged(const Qt::CheckState &arg1)
{
    if(ui->checkBox_transmit->isChecked())
    {
        autoSendTimer->start(ui->spinBox->value());
        ui->spinBox->setEnabled(false);
    }
    else
    {
        autoSendTimer->stop();
        ui->spinBox->setEnabled(true);
    }
}


void Widget::on_textEdit_receive_textChanged()
{
    QTextEdit *textEdit = ui->textEdit_receive;
    QString content = textEdit->toPlainText();

    // 检查是否超出长度限制
    if (content.length() > MAX_TEXT_LENGTH) {
        // 截断旧内容，保留最后 MAX_TEXT_LENGTH 个字符
        QString newContent = content.right(MAX_TEXT_LENGTH / 2);

        // 冻结界面刷新（提升性能）
        textEdit->blockSignals(true);
        textEdit->setPlainText(newContent);
        textEdit->blockSignals(false);

        // 移动光标到底部（保持滚动位置）
        textEdit->moveCursor(QTextCursor::End);
    }
}


void Widget::on_pushButton_clicked()
{


}


void Widget::on_checkBox_THEX_clicked()
{
    bool cl = ui->checkBox_THEX->isChecked();
    if(cl)
    {
        ui->checkBox_endl->setCheckState(Qt::Unchecked);
        ui->checkBox_endl->setVisible(false);
    }
    else
    {
        ui->checkBox_endl->setVisible(true);
    }
}

