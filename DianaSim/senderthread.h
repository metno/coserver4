#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <vector>
#include <QThread>
#include "dianaclient.h"


class SenderThread: public QThread
{
private:
    std::vector<DianaClient*> list;
    bool doRun;

public:
    SenderThread(std::vector<DianaClient*>&);
    void run();
    void stop();

};

#endif // SENDERTHREAD_H
