#ifndef CONNECTTHREAD_H
#define CONNECTTHREAD_H

#include <QThread>
#include "dianaclient.h"

class ConnectThread : public QThread
{
    Q_OBJECT

private:
    std::vector<DianaClient*>& list;
    bool doRun;

public:
    ConnectThread(std::vector<DianaClient*>& l);
    void run();
    void stop();

signals:
    void dianaConnect(DianaClient*);
};

#endif // CONNECTTHREAD_H
