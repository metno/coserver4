/** @file CoSocket.h
 * @author Martin Lilleeng Sætra <martinls@met.no>
 *
 * coserver4
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

#ifndef COSERVER4_COSOCKET
#define COSERVER4_COSOCKET 1

#include <coserver/miMessage.h>
#include <coserver/miMessageIO.h>

#include <QLocalSocket>
#include <QObject>
#include <QTcpSocket>

#include <memory>
#include <set>

class miMessageIO;

class CoSocket : public QObject
{
    Q_OBJECT

public:
    typedef std::set<int> peers_t;

public:
    CoSocket(int id, QObject* parent);
    ~CoSocket();

    void setSocket(QLocalSocket* local);
    void setSocket(QTcpSocket* tcp);

    void setTypeUserName(const QString& type, const QString& userId, const QString& name);

    bool hasTypeUserName()
        { return mHasTypeUserName; }

    int id() const
        { return mId; }

    const QString& getUserId();

    const QString& getType();

    void setName(const QString&);
    const QString& getName();

    void close();
    bool isClosed();
    bool isValid();

    int protocolVersion() const
        { return io->protocolVersion(); }

    void setProtocolVersion(int pv)
        { io->setProtocolVersion(pv); }

    bool usePeers() const
        { return protocolVersion() > 0; }

    void removePeer(int id)
        { mPeers.erase(id); }

    void setPeers(peers_t& p)
        { mPeers = p; }

    bool matchUser(CoSocket* p) const;
    bool isPeer(CoSocket* p) const;

    /**
     * Sends message to client.
     * @param msg The message
     */
    void sendMessage(int fromId, const miQMessage& msg);


private Q_SLOTS:
    /**
     * Read new incoming message.
     */
    void readNew();

    /**
     * Called when socket is disconnected.
     */
    void connectionClosed();
    void aboutToClose();

    void tcpError(QAbstractSocket::SocketError e);
    void localError(QLocalSocket::LocalSocketError e);

Q_SIGNALS:
    void connectionClosed(CoSocket* client);
    void receivedMessage(CoSocket* client, const ClientIds& toIds, const miQMessage &qmsg);

private:
    QTcpSocket* tcpSocket;
    QLocalSocket* localSocket;
    std::auto_ptr<miMessageIO> io;
    bool closed;

    int mId;
    bool mHasTypeUserName;
    QString userId;
    QString type;
    QString name;
    peers_t mPeers;
};

#endif // COSERVER4_COSOCKET
