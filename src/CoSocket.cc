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
#include <CoServer4.h>

CoSocket::CoSocket(int sock, QObject *parent) : QTcpSocket(parent) {
#ifdef HAVE_LOG4CXX
	logger = log4cxx::Logger::getLogger("coserver4.CoSocket"); ///< LOG4CXX init
#endif
	blockSize = 0;
	userId = "";

	setSocketDescriptor(sock);

	connect(this, SIGNAL(readyRead()), SLOT(readNew()));
	connect(this, SIGNAL(disconnected()), SLOT(connectionClosed()));
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
	CoServer4* server = (CoServer4*)parent();
	QDataStream in(this);
	in.setVersion(QDataStream::Qt_4_0);

	// make sure that the whole message has been written
#ifdef _DEBUG
	cout << "CoSocket::readNew: bytesAvailable(): " << bytesAvailable() << endl;
#endif


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
        in >> tmpcommand; msg.command = tmpcommand.toLatin1().constData();
        in >> tmpdescription; msg.description = tmpdescription.toLatin1().constData();
        in >> tmpcommondesc; msg.commondesc = tmpcommondesc.toLatin1().constData();
        in >> tmpcommon; msg.common = tmpcommon.toLatin1().constData();
        in >> tmpclientType; msg.clientType = tmpclientType.toLatin1().constData();
        in >> tmpco; msg.co = tmpco.toLatin1().constData();
	in >> size; // NOT A FIELD IN MIMESSAGE (TEMP ONLY)
	for (int i = 0; i < size; i++) {
		in >> tmpdata;
                msg.data.push_back(tmpdata.toLatin1().constData());
	}

#ifdef _DEBUG
	cout << "miMessage in CoSocket::readNew() (RECV)" << endl;
	cout << msg.content() << endl;
#endif

	// process message
	server->serve(msg, this);

	blockSize = 0;

	// more unread messages on socket?
	if(bytesAvailable() > 0)
		readNew();
}

void CoSocket::sendMessage(miMessage &msg) {
	if (state() == QTcpSocket::ConnectedState) {
#ifdef _DEBUG
		cout << "miMessage in CoSocket::sendMessage() (SEND)" << endl;
		cout << msg.content() << endl;
#endif

		QByteArray block;
		QDataStream out(&block, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_0);

		// send message to server
		out << (quint32)0;

		out << msg.to;
		out << msg.from;
                out << msg.command;
                out << msg.description;
                out << msg.commondesc;
                out << msg.common;
                out << msg.clientType;
                out << msg.co;
		out << (quint32)msg.data.size(); // NOT A FIELD IN MIMESSAGE (TEMP ONLY)
		for (int i = 0; i < msg.data.size(); i++)
                        out << msg.data[i];

		out.device()->seek(0);
		out << (quint32)(block.size() - sizeof(quint32));

		write(block);
		flush();
	} else {
		LOG4CXX_ERROR(logger, "Error sending message");
	}
}

void CoSocket::connectionClosed() {
	CoServer4* server = (CoServer4*)parent();
	server->killClient(this);
}
