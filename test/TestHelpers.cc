
#include "TestHelpers.h"

#include <coserver/CoClient.h>

#include <QCoreApplication>
#include <QTimer>

const int test_wait_default = 100; // ms

void test_wait(int ms)
{
    QTimer timer;
    timer.setSingleShot(true);
    timer.start(ms + 10);
    while (timer.isActive()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        usleep(10*1000 /*us*/);
    }
}

// ########################################################################

MessageReceiver::MessageReceiver(CoClient* client, QObject* parent)
    : QObject(parent)
{
    connect(client, SIGNAL(receivedMessage(int, const miQMessage&)),
            SLOT(onReceivedMessage(int, const miQMessage&)));
}

void MessageReceiver::onReceivedMessage(int from, const miQMessage& msg)
{
    mFrom << from;
    mMessages << msg;
}

void MessageReceiver::clear()
{
    mFrom.clear();
    mMessages.clear();
}

//########################################################################

ClientFixture::ClientFixture(const QString& us)
    : url(us)
    , server(url, false)
{
}

ClientFixture::~ClientFixture()
{
    for (int i=0; i<receivers.count(); ++i)
        delete receivers.at(i);
    for (int i=0; i<clients.count(); ++i) {
        CoClient* c = clients.at(i);
        c->disconnectFromServer();
        test_wait();
        delete c;
    }
}

ClientFixture::CoClient_x ClientFixture::addClient(MessageReceiver_x& m, const QString& type, const QString& user)
{
    CoClient* c = new CoClient(type, url);
    c->setUserId(user);
    c->connectToServer();
    test_wait();

    m = new MessageReceiver(c);

    clients << c;
    receivers << m;

    return c;
}

void ClientFixture::clearCounts()
{
    test_wait();
    for (int i=0; i<receivers.count(); ++i)
        receivers.at(i)->clear();
}
