/**
 * coserver2
 * @author Martin Lilleeng Sætra <martinls@met.no>
 * 
 * $Id: CoServer2.cc,v 1.22 2007/09/04 11:00:40 martinls Exp $
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

// TODO: Add support for multiple servers active on different ports (on the same node)
// TODO: Add support for multiple clients per server

// Qt-includes
#include <QtNetwork>
#include <QTextStream>
#include <qapplication.h>

#include <iostream>
#include <stdlib.h>

#include <qUtilities/QLetterCommands.h>
#include "CoServer2.h"

CoServer2::CoServer2(quint16 port,
        bool dm, bool vm) : QTcpServer() {
    id = 0;
    visualMode = vm;
    dynamicMode = dm;
    
    logger = log4cxx::Logger::getLogger("coserver2.CoServer2"); ///< LOG4CXX init
    log4cxx::PropertyConfigurator::configure("log4cxx.properties");

    if(dynamicMode)
        cout << "Started" << endl;

    listen(QHostAddress::Any, port);

    if(isListening()) {
    	LOG4CXX_INFO(logger, "coserver2 listening on port " << port);
    } else {
    	LOG4CXX_ERROR(logger, "Failed to bind to port");
    }
    
    console = new CoConsole();
    if (visualMode) {
		console->show();
	}
}

void CoServer2::incomingConnection(int sock) {
	// fetch incoming connection (socket)
    CoSocket *client = new CoSocket(sock, this);
    
    // add to list of clients
    int id = newId();
    client->setId(id);
    clients[id] = client;

    ostringstream text;
    text << "New client connected and assigned id " << id;
    LOG4CXX_INFO(logger, "New client connected and assigned id " << id);
    console->log(text.str());
    
#ifdef _DEBUG
    cout << "New total number of clients: " << (int) clients.size() << endl;
#endif
}

void CoServer2::broadcast(miMessage &msg) {
	map<int, CoSocket*>::iterator it;
	for(it = clients.begin(); it != clients.end(); it++) {		
		stringstream s;
		s << it->first; ///< find current id for iterator element
		string clientId(s.str());
		string id;
					
		// do not send message back to sender
		if(msg.commondesc == "id:type") {
			id = (msg.common.split(":"))[0]; ///< extract id from the message to be broadcast
		} else {
			stringstream out;
			out << msg.from;
			id = out.str();
		}
		
		if(!(id == clientId))
			it->second->sendMessage(msg);
		
	}
}

void CoServer2::killClient(CoSocket *client) {
	// tell the other connected clients of the disconnecting client
    QString data;
    QTextStream s(&data, QIODevice::WriteOnly);
    s << client->getId() << ':' << QString(client->getType().c_str());
    
    miMessage  update;
    update.to         = -1;
    update.from       = 0;
    update.command    = qmstrings::removeclient;
    update.commondesc = "id:type";
    update.common     = (miString)data.toAscii().data();
    
    serve(update);
    
    ostringstream text;
    text << "Client " << client->getId() << " disconnected";
    LOG4CXX_INFO(logger, "Client " << client->getId() << " disconnected");
    console->log(text.str());
    
    // remove client from the list of clients
    clients.erase(client->getId());

    // exit if no more clients are connected
	if (dynamicMode && clients.size() <= 0)
		QApplication::exit(1);
}

void CoServer2::serve(miMessage &msg, CoSocket *client) {
    if (msg.to == -1) {
    	// broadcast message
    	if (client != 0) {
    		msg.from = client->getId();
    	} else {
    		msg.from = 0;
    	}
    	broadcast(msg);
    	LOG4CXX_DEBUG(logger, "Broadcast message relayed");
	} else if (msg.to == 0 && client != 0) {
		// message is addressed to server
		internal(msg, client);
		LOG4CXX_DEBUG(logger, "Server message received");
	} else {
		// send message to the addressed client
		if (client != 0) {
			msg.from = client->getId();
		} else {
			msg.from = 0;
		}
		
		clients[msg.to]->sendMessage(msg);
		LOG4CXX_DEBUG(logger, "Direct message relayed");

		if (visualMode && msg.from) {
			cerr << msg.content();
		}
	}
}

int CoServer2::newId() {
    return ++id;
}

bool CoServer2::ready() {
	return isListening();
}

void CoServer2::internal(miMessage &msg, CoSocket *client) {
    if (msg.command == "SETTYPE") {
    	// set type in list of clients
        client->setType(msg.data[0].cStr());
        
        ostringstream text;
        text << "New client is of type " << client->getType().c_str();
        LOG4CXX_INFO(logger, "New client is of type " << client->getType().c_str());
        console->log(text.str());
        
        // broadcast the type of new connected client
        QString data;
        QTextStream s(&data, QIODevice::WriteOnly);
        s << client->getId() << ':' << QString(client->getType().c_str());
      
        miMessage update;
        update.to         = -1;
        update.from       = 0;
        update.command    = qmstrings::newclient;
        update.commondesc = "id:type";
        update.common     = (miString)data.toAscii().data();
        
        serve(update);
        
        // sends the list of already connected clients to the new client
        if(clients.size() > 1) {
        	map<int, CoSocket*>::iterator it;
        	for(it = clients.begin(); it != clients.end(); it++) {
        		CoSocket *tclient = it->second;
        		
        		// do not send message to yourself
        		if (!(tclient->getId() == client->getId())) {
					QString data;
					QTextStream s(&data, QIODevice::WriteOnly);
					s << tclient->getId() << ':'<< QString(tclient->getType().c_str());

					miMessage update;
					update.to = client->getId();
					update.from = 0;
					update.command = qmstrings::newclient;
					update.commondesc = "id:type";
					update.common = (miString)data.toAscii().data();

					serve(update);
				}
			}
		}
  }
}
