#ifndef DIANACLIENT_H
#define DIANACLIENT_H

#include <QWidget>
#include "ui_Client.h"

namespace Ui {
    class ClientWidget;
}

class DianaClient : public QWidget
{
    Q_OBJECT

public:
    DianaClient(QWidget *parent);
    ~DianaClient();

   Ui::ClientWidget *ui;
};

#endif // DIANACLIENT_H
