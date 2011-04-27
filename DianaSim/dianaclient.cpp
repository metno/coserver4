#include "dianaclient.h"

DianaClient::DianaClient(QWidget *parent):
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);
    ui->pushButton->connectClient("Diana", "dd");
}

DianaClient::~DianaClient()
{
    delete ui;
}
