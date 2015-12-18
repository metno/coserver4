
#ifndef TESTHELPERS_H
#define TESTHELPERS_H 1

#include "CoServer4.h"
#include <coserver/CoClient.h>
#include <coserver/miMessage.h>

#include <QtCore/QList>
#include <QtCore/QObject>

#include <gtest/gtest.h>


extern const int test_wait_default;
void test_wait(int ms = test_wait_default);

// ########################################################################

class MessageReceiver : public QObject {
    Q_OBJECT;

public:
    MessageReceiver(CoClient* client, QObject* parent=0);

    int count() const
        { return mFrom.size(); }

    int from(int idx) const
        { return mFrom.at(idx); }

    const miQMessage& message(int idx) const
        { return mMessages.at(idx); }

    void clear();

public Q_SLOTS:
    void onReceivedMessage(int from, const miQMessage&);

private:
    QList<int> mFrom;
    QList<miQMessage> mMessages;
};

// ########################################################################

class ClientFixture : public ::testing::Test {
public:
    typedef CoClient* CoClient_x;
    typedef MessageReceiver* MessageReceiver_x;

    ClientFixture(const QString& url);
    ~ClientFixture();

    CoClient_x addClient(MessageReceiver_x& m, const QString& type, const QString& user);
    void clearCounts();

    QUrl url;
    CoServer4 server;

    QList<CoClient_x> clients;
    QList<MessageReceiver_x> receivers;
};

#endif // TESTHELPERS_H
