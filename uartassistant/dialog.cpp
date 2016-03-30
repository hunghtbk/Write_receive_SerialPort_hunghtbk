#include "qextserialport.h"
#include "qextserialenumerator.h"
#include "dialog.h"
#include "ui_dialog.h"
#include <QtCore>
int CheckcmbModulation_1(QString);
int CheckcmbModulation_2(QString);
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //! [0]
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        ui->portBox1->addItem(info.portName);
    //make sure user can input their own port name!
    ui->portBox1->setEditable(true);

    ui->baudRateBox1->addItem("1200", BAUD1200);
    ui->baudRateBox1->addItem("2400", BAUD2400);
    ui->baudRateBox1->addItem("4800", BAUD4800);
    ui->baudRateBox1->addItem("9600", BAUD9600);
    ui->baudRateBox1->addItem("19200", BAUD19200);
    ui->baudRateBox1->addItem("38400", BAUD38400);
    ui->baudRateBox1->setCurrentIndex(3);

    ui->parityBox1->addItem("NONE", PAR_NONE);
    ui->parityBox1->addItem("ODD", PAR_ODD);
    ui->parityBox1->addItem("EVEN", PAR_EVEN);

    ui->dataBitsBox1->addItem("5", DATA_5);
    ui->dataBitsBox1->addItem("6", DATA_6);
    ui->dataBitsBox1->addItem("7", DATA_7);
    ui->dataBitsBox1->addItem("8", DATA_8);
    ui->dataBitsBox1->setCurrentIndex(3);

    ui->stopBitsBox1->addItem("1", STOP_1);
    ui->stopBitsBox1->addItem("2", STOP_2);

    ui->queryModeBox1->addItem("Polling", QextSerialPort::Polling);
    ui->queryModeBox1->addItem("EventDriven", QextSerialPort::EventDriven);
    //! [0]

    ui->led1->turnOff();
    ui->led1_2->turnOff();
    _On=false;

    timer = new QTimer(this);
    timer->setInterval(40);
    //! [1]
    PortSettings settings = {BAUD9600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    port = new QextSerialPort(ui->portBox1->currentText(), settings, QextSerialPort::Polling);
    //! [1]

    enumerator = new QextSerialEnumerator(this);
    enumerator->setUpNotifications();

    connect(ui->baudRateBox1, SIGNAL(currentIndexChanged(int)), SLOT(onBaudRateChanged(int)));
    connect(ui->cmbModulation1, SIGNAL(currentIndexChanged(int)), SLOT(onModulation1(int)));
    connect(ui->cmbModulation2, SIGNAL(currentIndexChanged(int)), SLOT(onModulation2(int)));
    connect(ui->parityBox1, SIGNAL(currentIndexChanged(int)), SLOT(onParityChanged(int)));
    connect(ui->dataBitsBox1, SIGNAL(currentIndexChanged(int)), SLOT(onDataBitsChanged(int)));
    connect(ui->stopBitsBox1, SIGNAL(currentIndexChanged(int)), SLOT(onStopBitsChanged(int)));
    connect(ui->queryModeBox1, SIGNAL(currentIndexChanged(int)), SLOT(onQueryModeChanged(int)));
    connect(ui->timeoutBox1, SIGNAL(valueChanged(int)), SLOT(onTimeoutChanged(int)));
    connect(ui->portBox1, SIGNAL(editTextChanged(QString)), SLOT(onPortNameChanged(QString)));
    connect(ui->openCloseButton1, SIGNAL(clicked()), SLOT(onOpenCloseButtonClicked()));
    connect(ui->sendButton1, SIGNAL(clicked()), SLOT(onSendButtonClicked()));
    connect(timer, SIGNAL(timeout()), SLOT(onReadyRead()));
    connect(port, SIGNAL(readyRead()), SLOT(onReadyRead()));

    connect(enumerator, SIGNAL(deviceDiscovered(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
    connect(enumerator, SIGNAL(deviceRemoved(QextPortInfo)), SLOT(onPortAddedOrRemoved()));

    setWindowTitle(tr("Embedded Networking Research Group Lab 411"));


}

Dialog::~Dialog()
{
    delete ui;
    delete port;
}

void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Dialog::onPortNameChanged(const QString & /*name*/)
{
    if (port->isOpen()) {
        port->close();
        ui->led1->turnOff();
    }
}
//! [2]
void Dialog::onBaudRateChanged(int idx)
{
    port->setBaudRate((BaudRateType)ui->baudRateBox1->itemData(idx).toInt());
}

void Dialog::onParityChanged(int idx)
{
    port->setParity((ParityType)ui->parityBox1->itemData(idx).toInt());
}

void Dialog::onDataBitsChanged(int idx)
{
    port->setDataBits((DataBitsType)ui->dataBitsBox1->itemData(idx).toInt());
}

void Dialog::onStopBitsChanged(int idx)
{
    port->setStopBits((StopBitsType)ui->stopBitsBox1->itemData(idx).toInt());
}

void Dialog::onQueryModeChanged(int idx)
{
    port->setQueryMode((QextSerialPort::QueryMode)ui->queryModeBox1->itemData(idx).toInt());
}

void Dialog::onTimeoutChanged(int val)
{
    port->setTimeout(val);
}
//! [2]
//! [3]
void Dialog::onOpenCloseButtonClicked()
{
    if (!port->isOpen()) {
        port->setPortName(ui->portBox1->currentText());
        port->open(QIODevice::ReadWrite);
    }
    else {
        port->close();
    }

    //If using polling mode, we need a QTimer
    if (port->isOpen() && port->queryMode() == QextSerialPort::Polling)
        timer->start();
    else
        timer->stop();

    //update led's status
    ui->led1->turnOn(port->isOpen());
}
//! [3]
//! [4]
void Dialog::onSendButtonClicked()
{

    if (port->isOpen() && !ui->sendEdit1->toPlainText().isEmpty())
        port->write(ui->sendEdit1->toPlainText().toLatin1());
        ui->sendEdit1->clear();
}

void Dialog::onReadyRead()
{
    if (port->bytesAvailable()) {
        ui->recvEdit1->moveCursor(QTextCursor::End);
        ui->recvEdit1->insertPlainText(QString::fromLatin1(port->readAll()));
    }
}

void Dialog::onModulation1(int idx)
{

}

void Dialog::onModulation2(int idx)
{

}

void Dialog::onPortAddedOrRemoved()
{
    QString current = ui->portBox1->currentText();

    ui->portBox1->blockSignals(true);
    ui->portBox1->clear();
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        ui->portBox1->addItem(info.portName);

    ui->portBox1->setCurrentIndex(ui->portBox1->findText(current));

    ui->portBox1->blockSignals(false);
}

//! [4]

void Dialog::on_pushButton_clicked()
{
    qApp->quit();
}

void Dialog::on_btnSet_clicked()
{
    QString LENH;
    int _cmd1=CheckcmbModulation_1(ui->cmbModulation1->currentText());
    int _cmd2=CheckcmbModulation_2(ui->cmbModulation2->currentText());
    if(_cmd2==1){
        switch(_cmd1){
        case 0: LENH="m500ctl 0 mdm 0\n";break;
        case 1: LENH="m500ctl 0 mdm 1\n";break;
        case 2: LENH="m500ctl 0 mdm 2\n";break;
        case 3: LENH="m500ctl 0 mdm 3\n";break;
        case 4: LENH="m500ctl 0 mdm 4\n";break;
        case 6: LENH="m500ctl 0 mdm 6\n";break;
        default: break;
        }
    }
    else if(_cmd2==2){
        switch(_cmd1){
        case 0: LENH="m500ctl 0 ddm 0\n";break;
        case 1: LENH="m500ctl 0 ddm 1\n";break;
        case 2: LENH="m500ctl 0 ddm 2\n";break;
        case 3: LENH="m500ctl 0 ddm 3\n";break;
        case 4: LENH="m500ctl 0 ddm 4\n";break;
        case 6: LENH="m500ctl 0 ddm 6\n";break;
        default: break;
        }
    }
   // ui->sendEdit->document()->setPlainText(LENH);
      QByteArray ba = LENH.toLatin1();
      const char *c_str2 = ba.data();
    port->write(c_str2);
    qDebug()<<LENH;
}
int CheckcmbModulation_1(QString _cmd){
    if(!_cmd.compare("BPSK")) return 0;
    if(!_cmd.compare("QPSK")) return 1;
    if(!_cmd.compare("0QPSK")) return 2;
    if(!_cmd.compare("8PSK")) return 3;
    if(!_cmd.compare("8QAM")) return 4;
    if(!_cmd.compare("16QAM")) return 6;

}
int CheckcmbModulation_2(QString _cmd){
    if(!_cmd.compare("Mod")) return 1;
    if(!_cmd.compare("Demod")) return 2;
}

void Dialog::on_pushButton_2_clicked()
{
    if(!_On){
        ui->led1_2->turnOn(true);
        _On=true;
        }
    else {
        ui->led1_2->turnOff(true);
        _On=false;
    }
}

void Dialog::on_pushButton_5_clicked()
{
    QString LENH;
    QString x=ui->lneDataRate1->text();
    int _cmd2=CheckcmbModulation_2(ui->cmbModulation2->currentText());
    if(_cmd2==1){
        LENH="mq mdr "+x+"\n";
    }
    else if(_cmd2==2){
        LENH="mq ddr "+x+"\n";
    }
    qDebug()<<LENH;
    QByteArray ba = LENH.toLatin1();
    const char *c_str2 = ba.data();
    port->write(c_str2);
    qDebug()<<LENH;
}

void Dialog::on_btnSet1_clicked()
{
    QString LENH;
    int _cmd1=CheckcmbModulation_1(ui->cmbModulation1->currentText());
    int _cmd2=CheckcmbModulation_2(ui->cmbModulation2->currentText());
    if(_cmd2==1){
        switch(_cmd1){
        case 0: LENH="m500ctl 0 mdm 0\n";break;
        case 1: LENH="m500ctl 0 mdm 1\n";break;
        case 2: LENH="m500ctl 0 mdm 2\n";break;
        case 3: LENH="m500ctl 0 mdm 3\n";break;
        case 4: LENH="m500ctl 0 mdm 4\n";break;
        case 6: LENH="m500ctl 0 mdm 6\n";break;
        default: break;
        }
    }
    else if(_cmd2==2){
        switch(_cmd1){
        case 0: LENH="m500ctl 0 ddm 0\n";break;
        case 1: LENH="m500ctl 0 ddm 1\n";break;
        case 2: LENH="m500ctl 0 ddm 2\n";break;
        case 3: LENH="m500ctl 0 ddm 3\n";break;
        case 4: LENH="m500ctl 0 ddm 4\n";break;
        case 6: LENH="m500ctl 0 ddm 6\n";break;
        default: break;
        }
    }
   // ui->sendEdit->document()->setPlainText(LENH);
      QByteArray ba = LENH.toLatin1();
      const char *c_str2 = ba.data();
    port->write(c_str2);
    qDebug()<<LENH;
}
