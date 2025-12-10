#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "my_serial.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void set_pos();

private slots:
    void on_pushButton_save_clicked();

    void on_pushButton_read_clicked();

    void on_Serial_button_clicked();

    void on_refresh_serial_clicked();

    void on_pushButton_transmit_clicked();

private:
    Ui::Widget *ui;
    void resizeEvent(QResizeEvent *event);
    QString buffer;
    my_serial serial;
    QTimer* autoSendTimer;
    static const int MAX_TEXT_LENGTH = 100000;
 private slots :
   void handleSerialData();

   void on_checkBox_transmit_checkStateChanged(const Qt::CheckState &arg1);
   void on_textEdit_receive_textChanged();
   void on_pushButton_clicked();
   void on_checkBox_THEX_clicked();
};
#endif // WIDGET_H
