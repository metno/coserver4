#include "connectthread.h"
#include <QTime>

ConnectThread::ConnectThread(std::vector<DianaClient*>& l)
    :list(l), doRun(true)
{

}

void ConnectThread::run()
{
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    while (doRun) {
        if (list.size() > 0) {
            int pos = qrand() % list.size();
            DianaClient* cl = list[pos];

            emit dianaConnect(cl);
        }
        this->msleep(100);

    }
}

void ConnectThread::stop()
{
    doRun = false;
}
