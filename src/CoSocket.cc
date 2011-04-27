/**
 * coserver2
 * @author Martin Lilleeng Sætra <martinls@met.no>
 *
 * $Id: CoSocket.cc,v 1.13 2007/09/04 11:00:40 martinls Exp $
 *
 * Copyright (C) 2007 met.no
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

// Qt-includes
#include <QDataStream>
#include <QTextStream>

#include <iostream>

#include <CoSocket.h>
#include <miLogger/LogHandler.h>

#define PKG "coserver4.CoSocket"

using namespace miutil;

CoSocket::CoSocket(int sock, QObject *parent)
    : QTcpSocket(parent)
{
    MI_LOG & log = MI_LOG::getInstance(PKG".CoSocket");

    blockSize = 0;
    userId = "";
    closed = false;
    setSocketDescriptor(sock);

    connect(this, SIGNAL(readyRead()), SLOT(readNew()));
    connect(this, SIGNAL(disconnected()), SLOT(connectionClosed()));
    connect(this, SIGNAL(aboutToClose()), SLOT(aboutToClose()));
    // socket error
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

}

CoSocket::~CoSocket()
{

}

void CoSocket::setId(int id) {
    this->id = id;
}

int CoSocket::getId() {
    return id;
}

void CoSocket::setUserId(string id) {
    this->userId = id;
}

string CoSocket::getUserId() {
    return userId;
}

void CoSocket::setType(string type) {
    this->type = type;
}

string CoSocket::getType(void) {
    return type;
}

void CoSocket::readNew() {
    MI_LOG & log = MI_LOG::getInstance(PKG".readNew");
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_0);

    // make sure that the whole message has been written
    log.debugStream() << "bytesAvailable(): " << bytesAvailable();

    if (blockSize == 0) {
        if (bytesAvailable() < (int)sizeof(quint32))
            return;
        in >> blockSize;
    }

    if (bytesAvailable() < blockSize)
        return;

    // read incoming message
    miMessage msg;
    QString tmpcommand, tmpdescription, tmpcommondesc, tmpcommon,
            tmpclientType, tmpco, tmpdata;
    int size = 0;

    in >> msg.to;
    // server-side socket knows the id
    msg.from = id;
    in >> tmpcommand;
    msg.command = tmpcommand.toStdString();
    in >> tmpdescription;
    msg.description = tmpdescription.toStdString();
    in >> tmpcommondesc;
    msg.commondesc = tmpcommondesc.toStdString();
    in >> tmpcommon;
    msg.common = tmpcommon.toStdString();
    in >> tmpclientType;
    msg.clientType = tmpclientType.toStdString();
    in >> tmpco;
    msg.co = tmpco.toStdString();
    in >> size; // NOT A FIELD IN MIMESSAGE (METADATA ONLY)
    for (int i = 0; i < size; i++) {
        in >> tmpdata;
        msg.data.push_back(tmpdata.toStdString());
    }

    log.debugStream() << "miMessage in CoSocket::readNew() (RECV)";
    log.debugStream() << msg.content();

    // process message
    //server->serve(msg, this);

    emit newMessage(msg, getId());

    blockSize = 0;

    // more unread messages on socket?
    if(bytesAvailable() > 0)
        readNew();
}

bool CoSocket::isClosed()
{
    return closed;
}

void CoSocket::sendMessage(miMessage &msg) {
    MI_LOG & log = MI_LOG::getInstance(PKG".sendMessage");

    if (state() == QTcpSocket::ConnectedState && !closed) {
        log.debugStream() << "miMessage in CoSocket::sendMessage() (SEND)" << getId();
        log.debugStream() << msg.content();

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);

        // send message to server
        out << (quint32)0;

        out << msg.to;
        out << msg.from;
        out << QString(msg.command.cStr());
        out << QString(msg.description.cStr());
        out << QString(msg.commondesc.cStr());
        out << QString(msg.common.cStr());
        out << QString(msg.clientType.cStr());
        out << QString(msg.co.cStr());
        out << (quint32)msg.data.size(); // NOT A FIELD IN MIMESSAGE (TEMP ONLY)
        for (int i = 0; i < msg.data.size(); i++)
            out << QString(msg.data[i].cStr());

        out.device()->seek(0);
        out << (quint32)(block.size() - sizeof(quint32));

        write(block);
        // waitForBytesWritten(1000);

        //flush();
    } else {
        log.errorStream() << "Error sending message state: " << state() << " closed: " << closed;
    }
}

void CoSocket::socketError(QAbstractSocket::SocketError e)
{
    MI_LOG & log = MI_LOG::getInstance(PKG".socketError");

    if ( QAbstractSocket::RemoteHostClosedError == e ) {
        log.info("CoClient unexpected shutdown or crash");
    }
    else
        log.infoStream() << "Error when contacting coserver: " << e;
}

void CoSocket::aboutToClose()
{
    //cerr << "about to close" << getId() << endl;
    closed = true;
}

void CoSocket::connectionClosed() {
    //cerr <<  "connectionClosed " << getId() << endl;
    closed = true;
    emit connectionClosed(getId());
}
