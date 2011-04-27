/** @mainpage coserver4
 * @author Martin Lilleeng Sætra <martinls@met.no>
 *
 * $Id: CoServer4.h,v 1.9 2007/09/04 10:34:45 martinls Exp $
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

#ifndef _COSERVER4
#define _COSERVER4

// Qt-includes
#include <QTcpSocket>
#include <QTcpServer>
#include <QMutex>

#include <vector>
#include <map>

#ifdef HAVE_LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/basicconfigurator.h>
#else
#include <miLogger/logger.h>
#endif

#include <qUtilities/miMessage.h>
#include "CoSocket.h"
#include "CoConsole.h"

using namespace std;

class CoServer4: public QTcpServer {
Q_OBJECT

protected:
#ifdef HAVE_LOG4CXX
  log4cxx::LoggerPtr logger;
#endif

private:
  map<int, CoSocket*> clients;
  CoConsole *console;
  quint16 port;
  QMutex mutex;

  /**
   * Internal helper function. Sets type of new connecting client,
   * and transmit a list of already connected clients.
   * @param msg The message
   * @param client The originating client
   */
  void internal(miMessage &msg, CoSocket *client);
  int id;
  int newId();
  bool dynamicMode, visualMode;

  /**
   * Broadcasts a message to all connected clients.
   * @param msg Message to broadcast
   */
  void broadcast(miMessage &msg, string userId = "");

  /**
   * Handles new connecting clients.
   */
  void incomingConnection(int);

  //int writePortToFile();
  
public:
  /**
   * CoServer4.
   * @param port Port to connect to
   * @param vm Run in visual (GUI) mode
   * @param dm Run in dynamic mode
   * @param logPropFile When given, log4cxx will use logPropFilename as properties file
   * @param logPropFilename The log4cxx properties file
   */
  CoServer4(quint16 port, bool vm, bool dm, bool logPropFile = false,
      miutil::miString logPropFilename = "");
    
  /**
   * Process incoming message.
   * @param l The message
   * @param client The originating client
   */
  void serve(miMessage &l, CoSocket* client = 0);

  /**
   * Kills a client, and then notifies the other clients of the event.
   * Will shut down coserver4 if in dynamicMode and no more
   * clients are connected.
   * @param client Client to remove
   */
  void killClient(CoSocket* client);
  bool ready(void);
  
  //int readPortFromFile(quint16& port);

  private slots:
    void connectionClosed(int id);
    void serve(miMessage &mi, int id);
    
};

#endif
