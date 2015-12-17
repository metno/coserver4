/** @file CoConsole.h
 * @author Martin Lilleeng Sætra <martinls@met.no>
 * 
 * coserver4
 * 
 * $Id: CoConsole.h,v 1.5 2007/09/04 10:34:45 martinls Exp $
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

#ifndef _COCONSOLE
#define _COCONSOLE

#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialog>

class CoConsole : public QDialog {
	Q_OBJECT;

public:
	/**
	 * CoConsole.
	 */
	CoConsole();

	/**
	 * Displays log message.
	 * @param text The log message
	 */
	void log(const std::string &text);

private:
	QTextEdit *textfield;
	QTextCursor *cursor;
};

#endif
