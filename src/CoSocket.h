/** @file CoSocket.h
 * @author Martin Lilleeng S�tra <martinls@met.no>
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

#ifdef COSERVER
#include <coserver/miMessage.h>
#else
#include <qUtilities/miMessage.h>
#endif

using namespace std;

class CoSocket : public QTcpSocket
{
    Q_OBJECT

public:
    void setId(int);
    int getId(void);
    void setUserId(string);
    string getUserId(void);
    void setType(string);
    string getType(void);
    bool isClosed();

    /**
  * CoSocket.
  * @param sock The socketdescriptor
  * @param parent Parent object
  */
    CoSocket(int sock, QObject *parent);
    ~CoSocket();

    /**
  * Sends message to client.
  * @param msg The message
  */
    void sendMessage(miMessage &msg);

private:
    int id;
    string userId;
    quint32 blockSize;
    string type;
    volatile bool closed;

private slots:
    /**
  * Read new incoming message.
  */
    void readNew();

    /**
  * Called when socket is disconnected.
  */
    void connectionClosed();
    void aboutToClose();
    void socketError(QAbstractSocket::SocketError e);

signals:
    void connectionClosed(int id);
    void newMessage(miMessage &msg, int id);
};

#endif
