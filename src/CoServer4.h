// -*- c++ -*-
/** @mainpage coserver4
 * @author Martin Lilleeng Sætra <martinls@met.no>
 *
 * Copyright (C) 2013 met.no
 *
 * Contact information:
 * Norwegian Meteorological Institute
 * Box 43 Blindern
 * 0313 OSLO
 * NORWAY
 * email: diana@met.no
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _COSERVER4
#define _COSERVER4

#include "CoSocket.h"

#include <coserver/miMessage.h>

#include <QLocalServer>
#include <QTcpServer>

#include <vector>
#include <map>

class CoServer4: public QObject {
    Q_OBJECT

public:
    /**
     * @param url server url
     * @param dm Run in dynamic mode
     */
    CoServer4(const QUrl& url, bool dm);

    bool ready();

private Q_SLOTS:
    void onNewConnection();

    void onClientConnectionClosed(CoSocket* client);
    void onClientReceivedMessage(CoSocket* client, const ClientIds& toIds, const miQMessage &qmsg);

private:
    /*! check if this is a message for the server, and handle it
     * @return true iff it was a message for the server
     */
    bool messageToServer(CoSocket *client, const ClientIds& toIds, const miQMessage &qmsg);

    void handleSetType(CoSocket* client, const miQMessage& qmsg);
    void handleSetName(CoSocket* client, const miQMessage& qmsg);
    void handleSetPeers(CoSocket* client, const miQMessage& qmsg);
    void handleStopServer(CoSocket* client, const miQMessage& qmsg);

    int generateId();

    //! Broadcasts a message from one client to its peers
    void broadcastFromClient(CoSocket* sender, const ClientIds& toIds, const miQMessage &qmsg);

    void sendRemoveClient(CoSocket* client, const CoSocket::peers_t& toIds);
    void sendNewClient(CoSocket* client, CoSocket* to_whom);
    void sendNewClient(CoSocket* client, const CoSocket::peers_t& toIds);

    //! Broadcasts a message from the server to the list of clients
    void broadcastFromServer(const CoSocket::peers_t& toIds, const miQMessage &qmsg);

    void stopServer();

    CoSocket* findClient(int id);
    CoSocket::peers_t peerIds(CoSocket* client);
    CoSocket::peers_t clientsForUser(CoSocket* client, int protoMin, int protoMax);

private:
    QTcpServer* tcpServer;
    QLocalServer* localServer;
    bool dynamicMode;

    int nextId;

    typedef std::map<int, CoSocket*> clients_t;
    clients_t clients;
};

#endif
