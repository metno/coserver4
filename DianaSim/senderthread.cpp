#include "senderthread.h"
#include <QTime>

SenderThread::SenderThread(std::vector<DianaClient*>& l)
    : list(l), doRun(true)
{

}

void SenderThread::run()
{
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));


    while (doRun) {
        if (list.size() > 0) {
            int pos = qrand() % list.size();
            int to = qrand() % list.size();
            DianaClient* cl = list[pos];

            miMessage m;
            m.to=to;
            m.from=pos;
            m.command = "file_changed";
            m.command = "date";
            m.commondesc = "date";
            cl->ui->pushButton->sendMessage(m);
        }
        this->msleep(100);
    }
}

void SenderThread::stop() {
    doRun = false;
}
