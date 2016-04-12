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

#include "CoSocket.h"

#include <coserver/miMessage.h>
#include <coserver/miMessageIO.h>
#include <miLogger/LogHandler.h>

#define MILOGGER_CATEGORY "coserver4.CoSocket"
#include <qUtilities/miLoggingQt.h>

CoSocket::CoSocket(int id, QObject *parent)
    : QObject(parent)
    , tcpSocket(0)
    , localSocket(0)
    , closed(true)
    , mId(id)
    , mHasTypeUserName(false)
{
    METLIBS_LOG_SCOPE();
}

void CoSocket::setSocket(QLocalSocket* local)
{
    METLIBS_LOG_SCOPE();
    closed = false;
    localSocket = local;
    io.reset(new miMessageIO(local, true));

    connect(local, SIGNAL(readyRead()), SLOT(readNew()));
    connect(local, SIGNAL(disconnected()), SLOT(connectionClosed()));
    connect(local, SIGNAL(aboutToClose()), SLOT(aboutToClose()));
    connect(local, SIGNAL(error(QLocalSocket::LocalSocketError)),
            SLOT(localError(QLocalSocket::LocalSocketError)));
}

void CoSocket::setSocket(QTcpSocket* tcp)
{
    METLIBS_LOG_SCOPE();
    closed = false;
    tcpSocket = tcp;
    io.reset(new miMessageIO(tcp, true));

    connect(tcp, SIGNAL(readyRead()), SLOT(readNew()));
    connect(tcp, SIGNAL(disconnected()), SLOT(connectionClosed()));
    connect(tcp, SIGNAL(aboutToClose()), SLOT(aboutToClose()));
    connect(tcp, SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(tcpError(QAbstractSocket::SocketError)));
}

CoSocket::~CoSocket()
{
}

void CoSocket::setTypeUserName(const QString& t, const QString& u, const QString& n)
{
    if (!mHasTypeUserName) {
        mHasTypeUserName = true;
        type = t;
        userId = u;
        name = n;
    }
}

const QString& CoSocket::getUserId()
{
    return userId;
}

const QString& CoSocket::getType()
{
    return type;
}

void CoSocket::setName(const QString& n)
{
    name = n;
}

const QString& CoSocket::getName()
{
    return name;
}

bool CoSocket::matchUser(CoSocket* p) const
{
    METLIBS_LOG_SCOPE(LOGVAL(userId) << LOGVAL(p->userId));

    const bool match = (userId == p->userId)
            // broadcast client = empty userid
            || (userId.isEmpty() || p->userId.isEmpty());
    METLIBS_LOG_DEBUG(LOGVAL(match));
    return match;
}

bool CoSocket::isPeer(CoSocket* p) const
{
    METLIBS_LOG_SCOPE();
    if (!matchUser(p))
        return false;

    METLIBS_LOG_DEBUG(LOGVAL(mId) << LOGVAL(p->mId)
            << LOGVAL(usePeers()) << LOGVAL(p->usePeers())
            << LOGVAL(mPeers.count(p->mId)) << LOGVAL(p->mPeers.count(mId)));

    if (!usePeers() && !p->usePeers())
        return true;

    if (usePeers() && mPeers.count(p->mId))
        return true;

    if (p->usePeers() && p->mPeers.count(mId))
        return true;

    return false;
}

void CoSocket::readNew()
{
    METLIBS_LOG_SCOPE();

    int fromId;
    ClientIds toIds;
    miQMessage qmsg;
    while (isValid() && io->read(fromId, toIds, qmsg)) {
        METLIBS_LOG_DEBUG(qmsg);
        Q_EMIT receivedMessage(this, toIds, qmsg);
    }
}

bool CoSocket::isClosed()
{
    return closed;
}

bool CoSocket::isValid()
{
    if (!io.get())
        return false;
    if (tcpSocket)
        return tcpSocket->isValid();
    if (localSocket)
        return localSocket->isValid();
    return false;
}

void CoSocket::sendMessage(int fromId, const miQMessage &qmsg)
{
    METLIBS_LOG_SCOPE(LOGVAL(fromId));
    METLIBS_LOG_DEBUG(qmsg);

    if (!isValid()) {
        METLIBS_LOG_ERROR("trying to sending message while invalid");
        return;
    }

    io->write(fromId, clientId(mId), qmsg);
}

void CoSocket::tcpError(QAbstractSocket::SocketError e)
{
    METLIBS_LOG_SCOPE();

    if (QAbstractSocket::RemoteHostClosedError == e)
        METLIBS_LOG_INFO("client disconnect");
    else
        METLIBS_LOG_INFO("error " << e);
}

void CoSocket::localError(QLocalSocket::LocalSocketError e)
{
    METLIBS_LOG_SCOPE();

    if (QLocalSocket::PeerClosedError == e)
        METLIBS_LOG_INFO("client disconnect");
    else
        METLIBS_LOG_INFO("error " << e);
}

void CoSocket::close()
{
    METLIBS_LOG_SCOPE();
    io.reset(0);

    if (tcpSocket) {
        tcpSocket->close();
        tcpSocket->deleteLater();
        tcpSocket = 0;
    }

    if (localSocket) {
        localSocket->close();
        localSocket->deleteLater();
        localSocket = 0;
    }
}

void CoSocket::aboutToClose()
{
    METLIBS_LOG_SCOPE();
    closed = true;
}

void CoSocket::connectionClosed()
{
    METLIBS_LOG_SCOPE();
    closed = true;
    Q_EMIT connectionClosed(this);
}
