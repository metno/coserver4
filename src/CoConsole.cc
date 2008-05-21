/**
 * coserver2
 * @author Martin Lilleeng Sætra <martinls@met.no>
 * 
 * $Id: CoConsole.cc,v 1.5 2007/09/04 10:34:45 martinls Exp $
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
#include <QVBoxLayout>
#include <qapplication.h>

#include <CoConsole.h>

CoConsole::CoConsole() : QDialog() {
	setWindowTitle("CoServer4");
	
#ifdef HAVE_LOG4CXX
	logger = log4cxx::Logger::getLogger("coserver4.CoConsole"); ///< LOG4CXX init
#endif
	
	// set dialog layout
	QVBoxLayout *layout = new QVBoxLayout;
	setLayout(layout);
	
	QString info = QString("Log");
	QLabel *infolabel = new QLabel( info, this );
	infolabel->setAlignment(Qt::AlignCenter);
	layout->addWidget(infolabel);

	textfield = new QTextEdit(this);
	textfield->setReadOnly(true);
	layout->addWidget(textfield);
	textfield->insertPlainText("No clients connected yet\n");

	QPushButton *quit = new QPushButton("Quit", this);
	layout->addWidget(quit);

	connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));
}

void CoConsole::log(const string &text) {
	textfield->insertPlainText(text.c_str());
	textfield->insertPlainText("\n");
	textfield->ensureCursorVisible();
}
