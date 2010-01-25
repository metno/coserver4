/**
 * coserver4
 * @author Martin Lilleeng Sætra <martinls@met.no>
 *
 * $Id: CoServer4.cc,v 1.22 2007/09/04 11:00:40 martinls Exp $
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
#include "CoServer4.h"

using namespace miutil;
using namespace std;

CoServer4::CoServer4(quint16 port, bool dm, bool vm, bool logPropFile,
    string logPropFilename) :
  QTcpServer()
{
  id = 0;
  visualMode = vm;
  dynamicMode = dm;
  
#ifdef HAVE_LOG4CXX
  /// LOGGER
  miString logpro;
  if (logPropFile) {
    logpro = logPropFilename;
  }
#endif

#ifdef HAVE_LOG4CXX
  if ( logpro.exists() ) {
    log4cxx::PropertyConfigurator::configure(logpro.c_str());
  } else {
    log4cxx::BasicConfigurator::configure();
    log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getWarn());
  }

  logger = log4cxx::Logger::getLogger("coserver4.CoServer4");
#endif

  if (dynamicMode) {
	  cout << "Started" << endl;
	  #ifdef _DEBUG
	  	cerr << "Started dynamic mode" << endl;
	  #endif	  
  }
  
  listen(QHostAddress::Any, port);  

#ifdef HAVE_LOG4CXX
  if (isListening()) {
    LOG4CXX_INFO(logger, "coserver4 listening on port " << port);
  } else {
    LOG4CXX_ERROR(logger, "Failed to bind to port");
  }
#endif
  
  console = new CoConsole();
  if (visualMode) {
    console->show();
  }  
}
/*
int CoServer4::writePortToFile() {
	miString homePath = miString(getenv("HOME"));
	
	if (homePath.length() > 0) {
		FILE *pfile;
		pfile = fopen(miString(homePath + "/.coserver.port").cStr(), "w");
		if (pfile != NULL) {
			cerr << "File created" << endl;
			fputs(miString(miString(port) + "\n").cStr(), pfile);
			fclose(pfile);
		} else {
			cerr << "File NOT created" << endl;
		}
		return 0;	
	} else {
		cerr << "Path to users HOME not found." << endl;
		return 1;
	}

	return 0;
}
*/
/*
int CoServer4::readPortFromFile(quint16& port) {
	miString homePath = miString(getenv("HOME"));
	FILE *pfile;
	char fileContent[10];
	
	pfile = fopen(miString(homePath + "/.coserver.port").cStr(), "r");
	if (pfile == NULL) {
		cerr << "Error opening diana.port" << endl;
		return 1;
	} else {		
		fgets(fileContent, 10, pfile);
		puts(fileContent);
		fclose(pfile);
		port = miString(fileContent).toInt(0);
		
		cerr << "Port is set to: " << port << endl;
	}		
	return 0;	
}
*/
void CoServer4::incomingConnection(int sock)
{
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

void CoServer4::broadcast(miMessage &msg, int userId)
{

  LOG4CXX_DEBUG(logger, "broadcast userId: " << userId << endl);
  map<int, CoSocket*>::iterator it;
  for (it = clients.begin(); it != clients.end(); it++) {
    stringstream s;
    s << it->first; ///< find current id for iterator element
    string clientId(s.str());
    string id;

    // do not send message back to sender
    if (msg.commondesc == "id:type") {
      id = (msg.common.split(":"))[0]; ///< extract id from the message to be broadcast
    } else {
      stringstream out;
      out << msg.from;
      id = out.str();
    }

    if (!(id == clientId)) {
      // Send only to same userId if possible
      CoSocket* tclient = it->second;
      LOG4CXX_DEBUG(logger, "userId: " << userId << " getUserId(): " << tclient->getUserId());
      if (userId == 0 || tclient->getUserId() == 0 ||
        userId == tclient->getUserId()) {

        LOG4CXX_DEBUG(logger, "Broadcast to: " << clientId << endl << msg.content().c_str());

        it->second->sendMessage(msg);
      }
      else {
    	LOG4CXX_DEBUG(logger, "Ignored: " << userId << " " << tclient->getUserId());

      }
    }
  }
}

void CoServer4::killClient(CoSocket *client)
{
  // tell the other connected clients of the disconnecting client
  QString data;
  QTextStream s(&data, QIODevice::WriteOnly);
  s << client->getId() << ':' << QString(client->getType().c_str());

  miMessage update;
  update.to = -1;
  update.from = 0;
  update.command = qmstrings::removeclient;
  update.commondesc = "id:type";
  update.common = (miString) data.toAscii().data();

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

void CoServer4::serve(miMessage &msg, CoSocket *client)
{
  if (msg.to == -1) {
    int userId = 0;
    // broadcast message
    if (client != 0) {
	  msg.from = client->getId();
	} else {
      msg.from = 0;
    }
    internal(msg, client); ///< broadcast to server also
    if (client != 0) {
      userId = client->getUserId();
    }
    broadcast(msg, userId);
    LOG4CXX_DEBUG(logger, "Broadcast message relayed");
  } else if (msg.to == 0 && client != 0) {
    // message is addressed to server (not in use??)
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

int CoServer4::newId()
{
  return ++id;
}

bool CoServer4::ready()
{
  return isListening();
}


void CoServer4::internal(miMessage &msg, CoSocket *client)
{
  if (msg.command == "SETTYPE") {
    // set type in list of clients
    client->setType(msg.data[0].cStr());
    // set userId
    if (msg.commondesc == "userId") {
      int id = atoi(msg.common.cStr());
      client->setUserId(id);
	  LOG4CXX_INFO(logger, "New client from user: " << client->getUserId());
    }

    ostringstream text;
    text << "New client is of type " << client->getType().c_str();
    LOG4CXX_INFO(logger, "New client is of type " << client->getType().c_str());
    console->log(text.str());

    // broadcast the type of new connected client
    QString data;
    QTextStream s(&data, QIODevice::WriteOnly);
    s << client->getId() << ':' << QString(client->getType().c_str())
    << ':' << client->getUserId();

    miMessage update;
    update.to = -1;
    update.from = 0;
    update.command = qmstrings::newclient;
    update.commondesc = "id:type:uid";
    update.common = (miString) data.toAscii().data();
    broadcast(update, client->getUserId());
   // serve(update);

    // sends the list of already connected clients to the new client
    if (clients.size() > 1) {
      map<int, CoSocket*>::iterator it;
      for (it = clients.begin(); it != clients.end(); it++) {
        CoSocket *tclient = it->second;

        // do not send message to yourself
        if (!(tclient->getId() == client->getId())) {
        	// Or to other the my userId if not old clients possible
        	if (client->getUserId() == 0 || tclient->getUserId() == 0 ||
        		client->getUserId() == tclient->getUserId()) {
        	QString data;
          QTextStream s(&data, QIODevice::WriteOnly);
          s << tclient->getId() << ':' << QString(tclient->getType().c_str());

          miMessage update;
          update.to = client->getId();
          update.from = 0;
          update.command = qmstrings::newclient;
          update.commondesc = "id:type";
          update.common = (miString) data.toAscii().data();
          serve(update);
          }
        }
      }
    }
  }
}
