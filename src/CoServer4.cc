/**
 * coserver4
 * @author Martin Lilleeng Sætra <martinls@met.no>
 *
 * Copyright (C) 2007-2015 met.no
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

// TODO: Add support for multiple servers active on different ports (on the same node)
// TODO: Add support for multiple clients per server

#include "CoServer4.h"

#include <coserver/miMessage.h>
#include <coserver/QLetterCommands.h>

#include <QCoreApplication>
#include <QFile>
#include <QUrl>
#include <QtNetwork/QHostInfo>

#include <cstdlib>

#define MILOGGER_CATEGORY "coserver4.CoServer4"
#include <qUtilities/miLoggingQt.h>

using namespace std;

namespace {

const char STOP_COMMAND[] = "STOP_COSERVER";

void addRegisteredClient(miQMessage& reg, CoSocket* c)
{
    if (!c)
        return;

    if (reg.countDataColumns() < 3)
        reg.addDataDesc("id").addDataDesc("type").addDataDesc("name");

    reg.addDataValues(QStringList()
            << QString::number(c->id())
            << c->getType()
            << c->getName());
}

} // namespace

CoServer4::CoServer4(const QUrl& url, bool dm)
    : nextId(0)
    , dynamicMode(dm)
    , tcpServer(0)
    , localServer(0)
{
    METLIBS_LOG_SCOPE(LOGVAL(dynamicMode));

    if (url.scheme() == "co4") {
        QHostAddress address;
        const QString& host = url.host();
        if (host.isEmpty()) {
            METLIBS_LOG_INFO("listening on any interface");
            address = QHostAddress::Any;
        } else {
            const QHostInfo hi = QHostInfo::fromName(host);
            if (!hi.addresses().isEmpty()) {
                address = hi.addresses().first();
                METLIBS_LOG_INFO("listening on '" << host << "', which resolved to '" << address.toString() << "'");
            }
        }
        const int port = url.port(qmstrings::port);
        METLIBS_LOG_INFO("listening on port " << port);
        tcpServer = new QTcpServer(this);
        if (!tcpServer->listen(address, port)) {
            METLIBS_LOG_ERROR("cannot listen: " << tcpServer->errorString());
        }
        connect(tcpServer, SIGNAL(newConnection()),
                SLOT(onNewConnection()));
    } else if (url.scheme() == "local") {
        const QString path = url.path();
        QFile socket(path);
        if (socket.exists()) {
            METLIBS_LOG_INFO("socket '" << path << "' exists, trying to remove");
            if (!socket.remove()) {
                METLIBS_LOG_ERROR("failed to remove coserver4 file '" << path << "'");
            }
        }
        localServer = new QLocalServer(this);
        if (!localServer->listen(url.path())) {
            METLIBS_LOG_ERROR("cannot listen: " << localServer->errorString());
        }
        connect(localServer, SIGNAL(newConnection()),
                SLOT(onNewConnection()));
    } else {
        METLIBS_LOG_ERROR("unknown scheme '" << url.scheme() << "', cannot start server");
    }

    if (ready()) {
        METLIBS_LOG_INFO("coserver4 listening");
    } else {
        METLIBS_LOG_ERROR("failed to start coserver4");
    }
}

void CoServer4::onNewConnection()
{
    METLIBS_LOG_SCOPE();

    const int id = generateId();
    CoSocket *client = new CoSocket(id, this);
    if (tcpServer) {
        QTcpSocket* tcp = tcpServer->nextPendingConnection();
        client->setSocket(tcp);
    } else if (localServer) {
        QLocalSocket* local = localServer->nextPendingConnection();
        client->setSocket(local);
    } else {
        METLIBS_LOG_WARN("no server object, cannot accept connections.");
        delete client;
        return;
    }
    clients[client->id()] = client;

    connect(client, SIGNAL(connectionClosed(CoSocket*)),
            SLOT(onClientConnectionClosed(CoSocket*)));
    connect(client, SIGNAL(receivedMessage(CoSocket*, const ClientIds&, const miQMessage&)),
            SLOT(onClientReceivedMessage(CoSocket*, const ClientIds&, const miQMessage&)));

    METLIBS_LOG_INFO("New client connected and assigned id " << id);
    METLIBS_LOG_DEBUG("New total number of clients: " << clients.size());
}

void CoServer4::broadcastFromClient(CoSocket* sender, const ClientIds& toIds, const miQMessage &qmsg)
{
    METLIBS_LOG_SCOPE();
    METLIBS_LOG_DEBUG(LOGVAL(sender->id()) << LOGVAL(toIds.size()) << LOGVAL(qmsg.command()));
    const bool to_all = toIds.empty()
            || toIds.count(qmstrings::all)
            || (toIds.size() == 1 && *toIds.begin() == qmstrings::default_id);
    std::vector<CoSocket*> sendTo;
    if (!to_all)
        sendTo.reserve(toIds.size());
    for (clients_t::iterator it = clients.begin(); it != clients.end(); it++) {
        CoSocket* c = it->second;
        if (c == sender)
            continue;
        if (!to_all && toIds.count(c->id()) == 0) {
            METLIBS_LOG_DEBUG(LOGVAL(c->id()) <<  LOGVAL(toIds.empty()) << LOGVAL(toIds.count(c->id())));
            continue;
        }
        if (sender->isPeer(c)) {
            sendTo.push_back(c);
        } else {
            METLIBS_LOG_DEBUG("not a peer: " << LOGVAL(c->id()));
        }
    }

    for (std::vector<CoSocket*>::iterator it = sendTo.begin(); it != sendTo.end(); it++) {
        CoSocket* c = *it;
        c->sendMessage(sender->id(), qmsg);
    }
}

void CoServer4::onClientConnectionClosed(CoSocket* client)
{
    METLIBS_LOG_SCOPE();

    clients.erase(client->id());

    // tell the other connected clients of the disconnecting client
    CoSocket::peers_t inform;
    for (clients_t::iterator it = clients.begin(); it != clients.end(); it++) {
        CoSocket* c = it->second; // "client" is already erased
        if (client->isPeer(c))
            inform.insert(c->id());
        c->removePeer(c->id());
    }
    sendRemoveClient(client, inform);

    // tell other clients with use_peers that this one is gone; do not tell to other clients
    const CoSocket::peers_t clients_V1 = clientsForUser(client, 1, 1);
    if (!clients_V1.empty()) {
        miQMessage unregP(qmstrings::unregisteredclient);
        addRegisteredClient(unregP, client);
        broadcastFromServer(clients_V1, unregP);
    }

    METLIBS_LOG_INFO("Client " << client->id() << " disconnected");
    client->deleteLater();

    // exit if no more clients are connected, or if the server is shutting down
    if (clients.empty()) {
        if (dynamicMode || !ready())
            QCoreApplication::exit(1);
        else
            nextId = 0;
    }
}

void CoServer4::onClientReceivedMessage(CoSocket* client, const ClientIds& toIds, const miQMessage &qmsg)
{
    METLIBS_LOG_SCOPE(LOGVAL(qmsg.command()));
    if (toIds.count(qmstrings::default_id))
        METLIBS_LOG_WARN("client id " << client->id() << " sends command " << qmsg.command()
                << "to deprecated 'default_id'");
    if (!messageToServer(client, toIds, qmsg))
        broadcastFromClient(client, toIds, qmsg);
}

int CoServer4::generateId()
{
    // FIXME make sure that ids do not overflow
    return ++nextId;
}

bool CoServer4::ready()
{
    if (tcpServer)
        return tcpServer->isListening();
    else if (localServer)
        return localServer->isListening();
    else
        return false;
}

bool CoServer4::messageToServer(CoSocket *client, const ClientIds& toIds, const miQMessage &qmsg)
{
    METLIBS_LOG_SCOPE();

    const int toId1 = (toIds.size() == 1 && *toIds.begin() != qmstrings::default_id)
            ? *toIds.begin() : qmstrings::all;

    if (qmsg.command() == "SETTYPE") {
        handleSetType(client, qmsg);
    } else if (qmsg.command() == "SETNAME") {
        handleSetName(client, qmsg);
    } else if (qmsg.command() == "SETPEERS") {
        handleSetPeers(client, qmsg);
    } else if (qmsg.command() == STOP_COMMAND) {
        handleStopServer(client, qmsg);
    } else {
        if (toId1 == 0)
            METLIBS_LOG_WARN("unknown command '" << qmsg.command()
                    << "' from client id " << client->id() << " to server");
        return false;
    }
    if (toId1 != 0)
        METLIBS_LOG_WARN("client did not send " << qmsg.command() << " to id 0");
    return true;
}

void CoServer4::handleSetType(CoSocket* client, const miQMessage& qmsg)
{
    METLIBS_LOG_SCOPE();
    int idx = qmsg.findCommonDesc("protocolVersion");
    if (idx >= 0) {
        int v = qmsg.getCommonValue(idx).toInt();
        METLIBS_LOG_DEBUG(LOGVAL(v));
        client->setProtocolVersion(qmsg.getCommonValue(idx).toInt());
    }

    if (client->hasTypeUserName()) {
        METLIBS_LOG_ERROR("ignoring extra SETTYPE from client " << client->id());
        return;
    }

    QString cType, cUserId, cName;
    idx = qmsg.findCommonDesc("type");
    if (idx >= 0) {
        cType = qmsg.getCommonValue(idx);
    } else {
        idx = qmsg.findDataDesc("INTERNAL");
        if (idx >= 0 && qmsg.countDataRows() >= 1)
            cType = qmsg.getDataValue(0, idx);
    }

    idx = qmsg.findCommonDesc("userId");
    if (idx >= 0)
        cUserId = qmsg.getCommonValue(idx);

    idx = qmsg.findCommonDesc("name");
    if (idx >= 0)
        cName = qmsg.getCommonValue(idx);
    else
        cName = cType;

    client->setTypeUserName(cType, cUserId, cName);
    METLIBS_LOG_INFO("client " << client->id() << " has type '" << cType
            << "' userId '" << cUserId << "' name '" << cName << "'");

    if (client->usePeers()) {
        miQMessage reg(qmstrings::registeredclient);
        reg.addCommon("id", client->id());

        // send a list of ALL connected clients
        const CoSocket::peers_t same_user = clientsForUser(client, 0, 1);
        for (CoSocket::peers_t::const_iterator itP = same_user.begin(); itP != same_user.end(); ++itP)
            addRegisteredClient(reg, findClient(*itP));
        client->sendMessage(0, reg);
    } else {
        const CoSocket::peers_t clients_V0 = clientsForUser(client, 0, 0);
        sendNewClient(client, clients_V0);

        // sends the list of already connected clients to the new client
        for (CoSocket::peers_t::const_iterator itP = clients_V0.begin(); itP != clients_V0.end(); ++itP)
            sendNewClient(findClient(*itP), client);
    }

    // tell other clients with use_peers about this one; do not tell to other clients
    miQMessage regP(qmstrings::registeredclient);
    addRegisteredClient(regP, client);
    const CoSocket::peers_t clients_V1 = clientsForUser(client, 1, 1);
    broadcastFromServer(clients_V1, regP);
}

void CoServer4::handleSetName(CoSocket* client, const miQMessage& qmsg)
{
    METLIBS_LOG_SCOPE();
    int idx = qmsg.findCommonDesc("name");
    if (idx < 0)
        return;

    const QString& name = qmsg.getCommonValue(idx);
    if (name == client->getName())
        return;

    client->setName(name);
    METLIBS_LOG_INFO("New client name: " << client->getName());

    miQMessage update(qmstrings::renameclient);
    update.addCommon("id", client->id());
    update.addCommon("name", client->getName());

    const CoSocket::peers_t clients_V1 = clientsForUser(client, 1, 1);
    broadcastFromServer(clients_V1, update);
}

void CoServer4::handleSetPeers(CoSocket* client, const miQMessage& qmsg)
{
    METLIBS_LOG_SCOPE();
    int idx = qmsg.findDataDesc("peer_ids");
    METLIBS_LOG_DEBUG(LOGVAL(idx));
    if (idx < 0)
        return;

    const CoSocket::peers_t before = peerIds(client);

    CoSocket::peers_t listed;
    for (int r=0; r<qmsg.countDataRows(); ++r) {
        const int peerId = qmsg.getDataValue(r, idx).toInt();
        CoSocket* pc = findClient(peerId);
        if (!pc || !client->matchUser(pc)) {
            METLIBS_LOG_WARN("SETPEERS from " << client->id() << " with bad peer " << peerId);
            continue;
        }
        METLIBS_LOG_DEBUG("listed: " << peerId);
        listed.insert(peerId);
    }
    client->setPeers(listed);

    const CoSocket::peers_t after = peerIds(client);
    METLIBS_LOG_DEBUG(LOGVAL(before.size()) << LOGVAL(after.size()));

    CoSocket::peers_t added, removed;
    std::set_difference(before.begin(), before.end(), after.begin(), after.end(),
            std::insert_iterator<CoSocket::peers_t>(removed, removed.begin()));
    std::set_difference(after.begin(), after.end(), before.begin(), before.end(),
            std::insert_iterator<CoSocket::peers_t>(added, added.begin()));

    METLIBS_LOG_DEBUG(LOGVAL(added.size()) << LOGVAL(removed.size()));

    if (!removed.empty()) {
        sendRemoveClient(client, removed);

        CoSocket::peers_t to_sender;
        to_sender.insert(client->id());
        for (CoSocket::peers_t::const_iterator itP = removed.begin(); itP != removed.end(); ++itP) {
            METLIBS_LOG_DEBUG("removed " << *itP);
            sendRemoveClient(findClient(*itP), to_sender);
        }
    }
    if (!added.empty()) {
        sendNewClient(client, added);

        for (CoSocket::peers_t::const_iterator itP = added.begin(); itP != added.end(); ++itP) {
            METLIBS_LOG_DEBUG("added " << *itP);
            sendNewClient(findClient(*itP), client);
        }
    }
}

void CoServer4::handleStopServer(CoSocket* client, const miQMessage& qmsg)
{
    if (qmsg.command() == STOP_COMMAND
            && client->getName() == "coserver4_stop")
    {
        stopServer();
    }
}

void CoServer4::stopServer()
{
    METLIBS_LOG_SCOPE();

    // server owns all the client's sockets, we must delete the server later
    QTcpServer* tcps = 0;
    std::swap(tcpServer, tcps);
    QLocalServer* locals = 0;
    std::swap(localServer, locals);

    while (!clients.empty()) {
        CoSocket* c = clients.begin()->second;
        c->close();
    }

    delete tcps;
    delete locals;
}

CoSocket* CoServer4::findClient(int id)
{
    clients_t::iterator it = clients.find(id);
    if (it != clients.end())
        return it->second;
    else
        return 0;
}

CoSocket::peers_t CoServer4::peerIds(CoSocket* client)
{
    METLIBS_LOG_SCOPE(LOGVAL(client->id()));
    CoSocket::peers_t peers;
    for (clients_t::iterator it = clients.begin(); it != clients.end(); it++) {
        CoSocket* c = it->second;
        if (c != client && client->isPeer(c)) {
            METLIBS_LOG_DEBUG("peer: " << c->id());
            peers.insert(c->id());
        } else {
            METLIBS_LOG_DEBUG("no peer: " << c->id());
        }
    }
    return peers;
}

CoSocket::peers_t CoServer4::clientsForUser(CoSocket* client, int protoMin, int protoMax)
{
    CoSocket::peers_t all;
    for (clients_t::iterator it = clients.begin(); it != clients.end(); it++) {
        CoSocket* pc = it->second;
        if (pc != client && client->matchUser(pc)
                && (pc->protocolVersion() >= protoMin)
                && (pc->protocolVersion() <= protoMax))
        {
            all.insert(pc->id());
        }
    }
    return all;
}

void CoServer4::sendRemoveClient(CoSocket* client, const CoSocket::peers_t& to_whom)
{
    if (to_whom.empty())
        return;

    miQMessage update(qmstrings::removeclient);
    update.addCommon("id", client->id());
    update.addCommon("type", client->getType());

    broadcastFromServer(to_whom, update);
}

void CoServer4::sendNewClient(CoSocket* client, const CoSocket::peers_t& to_whom)
{
    if (to_whom.empty())
        return;

    miQMessage update(qmstrings::newclient);
    update.addCommon("id", client->id());
    update.addCommon("type", client->getType());
    update.addCommon("userId", client->getUserId());
    update.addCommon("name", client->getName());

    broadcastFromServer(to_whom, update);
}

void CoServer4::sendNewClient(CoSocket* client, CoSocket* to_whom)
{
    miQMessage update(qmstrings::newclient);
    update.addCommon("id",   client->id());
    update.addCommon("type", client->getType());
    update.addCommon("name", client->getName());
    to_whom->sendMessage(0, update);
}

void CoServer4::broadcastFromServer(const CoSocket::peers_t& toIds, const miQMessage &qmsg)
{
    METLIBS_LOG_SCOPE(LOGVAL(qmsg.command()) << LOGVAL(toIds.size()));
    for (CoSocket::peers_t::const_iterator itP = toIds.begin(); itP != toIds.end(); ++itP) {
        if (CoSocket* tc = findClient(*itP))
            tc->sendMessage(0, qmsg);
    }
}
