/** @file CoSocket.h
 * @author Martin Lilleeng Sætra <martinls@met.no>
 * 
 * coserver4
 * 
 * $Id: CoSocket.h,v 1.7 2007/09/04 10:34:45 martinls Exp $
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

#ifndef _COSOCKET
#define _COSOCKET

// Qt-includes
#include <QTcpSocket>

#ifdef HAVE_LOG4CXX
#include <log4cxx/logger.h>
#else
#include <miLogger/logger.h>
#endif

#include <qUtilities/miMessage.h>

using namespace std;

class CoSocket : public QTcpSocket {
	Q_OBJECT
	
protected:
#ifdef HAVE_LOG4CXX
	log4cxx::LoggerPtr logger;
#endif
	
public:
	void setId(int);
	int getId(void);
	void setUserId(int);
	int getUserId(void);
	void setType(string);
	string getType(void);

	/**
	 * CoSocket.
	 * @param sock The socketdescriptor
	 * @param parent Parent object
	 */
	CoSocket(int sock, QObject *parent);
	
	/**
	 * Sends message to client.
	 * @param msg The message
	 */
	void sendMessage(miMessage &msg);
	
private:
	int id;
	quint32 userId;
	quint32 blockSize;
	string type;
	
private slots:
	/**
	 * Read new incoming message.
	 */
	void readNew();
	
	/**
	 * Called when socket is disconnected.
	 */
	void connectionClosed();
};

#endif
