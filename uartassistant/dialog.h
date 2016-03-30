#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
    class Dialog;
}
class QTimer;
class QextSerialPort;
class QextSerialEnumerator;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    bool _On;

protected:
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void onPortNameChanged(const QString &name);
    void onBaudRateChanged(int idx);
    void onParityChanged(int idx);
    void onDataBitsChanged(int idx);
    void onStopBitsChanged(int idx);
    void onQueryModeChanged(int idx);
    void onTimeoutChanged(int val);
    void onOpenCloseButtonClicked();
    void onSendButtonClicked();
    void onReadyRead();
    void onModulation1(int idx);
    void onModulation2(int idx);

    void onPortAddedOrRemoved();

    void on_sendButton_clicked();

    void on_pushButton_clicked();

    void on_btnSet_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();

    void on_btnSet1_clicked();

    void on_sendButton1_clicked();

    void on_openCloseButton_clicked();

private:
    Ui::Dialog *ui;
    QTimer *timer;
    QextSerialPort *port;
    QextSerialEnumerator *enumerator;
};

#endif // DIALOG_H
