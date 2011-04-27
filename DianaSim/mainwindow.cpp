#include "mainwindow.h"
#include "dianaclient.h"
#include "ui_mainwindow.h"
#include "ui_Client.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    conThread = new ConnectThread(clients);
    sendThread = new SenderThread(clients);
    connect(conThread, SIGNAL(dianaConnect(DianaClient*)), SLOT(dianaConnect(DianaClient*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::startDianaClicked()
{
   nrClients = ui->nofClient->text().toInt();
    for (int i=0; i< nrClients; i++) {
        DianaClient *cl = new DianaClient(this);
        cl->ui->pushButton->connectClient("Diana","qserver");
        cl->ui->pushButton->connectToServer();
        clients.push_back(cl);
        ui->verticalClients->addWidget(cl);
    }
    if (!conThread->isRunning()) {
        conThread->start();
    }
    if (!sendThread->isRunning()) {
        sendThread->start();
    }

}

void MainWindow::stopDianaClicked()
{
    if (conThread->isRunning()) {
        conThread->stop();
    }
    if (sendThread->isRunning()) {
        sendThread->stop();
    }
}

void MainWindow::dianaConnect(DianaClient* cl)
{

    cl->ui->pushButton->connectToServer();
}
