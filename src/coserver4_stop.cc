
#include "coserver4_stop.h"

#include <coserver/CoClient.h>
#include <coserver/miMessage.h>
#include <coserver/QLetterCommands.h>

#include <QCoreApplication>
#include <QTimer>
#include <QUrl>

#define MILOGGER_CATEGORY "coserver4.stop"
#include <miLogger/miLogging.h>

#include <iostream>

// ========================================================================

ServerStopper::ServerStopper(const QUrl& url)
    : client(new CoClient("coserver4_stop", url, this))
{
    client->setAttemptToStartServer(false);

    METLIBS_LOG_INFO("will exits in 1.5s");
    QTimer::singleShot(1500, qApp, SLOT(quit()));

    connect(client, SIGNAL(connected()), SLOT(onConnected()));
    client->connectToServer();
}

void ServerStopper::onConnected()
{
    METLIBS_LOG_INFO("connected to server, waiting ...");
    QTimer::singleShot(500, this, SLOT(sendStopServer()));
}

void ServerStopper::sendStopServer()
{
    METLIBS_LOG_WARN("sending stop command to server");
    miQMessage qmsg("STOP_COSERVER");
    client->sendMessage(qmsg, clientId(0));
}

// ========================================================================

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    milogger::LoggingConfig log4cpp;

    QUrl url;
    if (argc == 2) {
        url.setUrl(argv[1]);
    } else if (argc == 1) {
        url.setScheme("co4");
        url.setHost("localhost");
        url.setPort(qmstrings::port);
    } else {
        std::cerr << argv[0] << " [coserver4-url]" << std::endl
                  << "    sends a stop command to coserver4" << std::endl;
        return 1;
    }

    ServerStopper stop(url);
    return app.exec();
}
