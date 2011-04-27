#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <QMainWindow>
#include "dianaclient.h"
#include "senderthread.h"
#include "connectthread.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    int nrClients;
    ConnectThread* conThread;
    SenderThread* sendThread;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void stopDianaClicked();
    void startDianaClicked();
    void dianaConnect(DianaClient*);

private:
    Ui::MainWindow *ui;
    std::vector<DianaClient*> clients;
};

#endif // MAINWINDOW_H
