/** @mainpage coserver2
 * @author Martin Lilleeng S�tra <martinls@met.no>
 * 
 * $Id: CoServer2.h,v 1.9 2007/09/04 10:34:45 martinls Exp $
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

#ifndef _COSERVER2
#define _COSERVER2

// Qt-includes
#include <QTcpSocket>
#include <QTcpServer>

#include <vector>
#include <map>

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/level.h> 

#include <qUtilities/miMessage.h>
#include "CoSocket.h"
#include "CoConsole.h"

using namespace std; 

class CoServer2 : public QTcpServer {
    Q_OBJECT
    
protected:
	log4cxx::LoggerPtr logger;
    
private:
    map<int, CoSocket*> clients;
    CoConsole *console;

    /**
     * Internal helper function. Sets type of new connecting client,
     * and transmit a list of already connected clients.
     * @param msg The message
     * @param client The originating client
     */
    void internal(miMessage &msg, CoSocket *client);
    int id ;
    int newId();
    bool dynamicMode,visualMode;
    
    /**
     * Broadcasts a message to all connected clients.
     * @param msg Message to broadcast
     */
    void broadcast(miMessage &msg);
    
    /**
     * Handles new connecting clients.
     */
    void incomingConnection(int);

public:
	/**
	 * CoServer2.
	 * @param port Port to connect to
	 * @param vm Run in visual (GUI) mode
	 * @param dm Run in dynamic mode
	 */
    CoServer2(quint16 port, bool vm, bool dm);

    /**
     * Process incoming message.
     * @param l The message
     * @param client The originating client
     */
    void serve(miMessage &l, CoSocket* client = 0);
    
    /**
     * Kills a client, and then notifies the other clients of the event.
     * Will shut down coserver2 if in dynamicMode and no more
     * clients are connected.
     * @param client Client to remove
     */
    void killClient(CoSocket* client);
    bool ready(void);
};

#endif
