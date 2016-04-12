
#ifndef COSERVER4_STOP_H
#define COSERVER4_STOP_H 1

#include <QObject>

class CoClient;
class QUrl;

class ServerStopper : public QObject {
    Q_OBJECT;

public:
    ServerStopper(const QUrl& url);

private Q_SLOTS:
    void onConnected();
    void sendStopServer();

private:
    CoClient* client;
};

#endif // COSERVER4_STOP_H
