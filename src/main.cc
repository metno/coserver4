/** @file main.cc
 * Main class for coserver4. Only used for initialization.
 * Run with -gui for GUI.
 * @author Martin Lilleeng Sætra <martinls@met.no>
 */

// Qt-includes
#include <QApplication>
#include <stdlib.h>
#include <puTools/miCommandLine.h>
#include <qUtilities/QLetterCommands.h>
#include "CoServer4.h"

using namespace std;
using namespace miutil;

int main(int argc, char *argv[])
{
#ifdef _DEBUG
  cerr << "Coserver main program called" << endl;
#endif

#ifdef _DEBUG
  cerr << "argc: " << argc << endl;

  for(int i=0;i<argc;i++) {
    cerr << "argv: " << miString(argv[i]) << endl;
  }
#endif

  // parsing commandline-arguments
  vector<miCommandLine::option> opt(5);
  miString logfile;
  CoServer4 *server;

  /*
  // TODO: Put these in configfile
bool portFromRange = false;
  vector<quint16> portRange(10);

  for (int i = 0; i < 9; i++) {
	  portRange[i] = (quint16)49151 + i;
  }
*/
  opt[0].flag = 'd';
  opt[0].alias = "dynamic";
  opt[0].hasArg = false;

  opt[1].flag = 'v';
  opt[1].alias = "visual";
  opt[1].hasArg = false;

  opt[2].flag = 'p';
  opt[2].alias = "port";
  opt[2].hasArg = true;

  opt[3].flag = 'L';
  opt[3].alias = "log4cxx-properties-file";
  opt[3].hasArg = true;

  miCommandLine cl(opt, argc, argv);

  quint16 port;
  quint16 fileport;

  if (cl.hasFlag('p')) {
    cerr << "P flag sent" << endl;
    //istringstream os((cl.arg('p'))[0]);
    //os >> port;

    if (cl.arg('p').size() >= 0) {
      port = miString(cl.arg('p')[0]).toInt(0);
    } else {
#ifdef _DEBUG
      cerr << "cl.arg('p').size() == 0" << cl.arg('p').size() << endl;
#endif
      port = qmstrings::port;
    }
/*  } else if (server->readPortFromFile(fileport) == 0) {
	  cerr << "Port read from file: " << fileport << endl;
	  port = fileport;
  //} else if (portFromRange == false) {
   */
  } else {
#ifdef _DEBUG
    cerr << "Flag p not set!!!"  << endl;
#endif
    port = qmstrings::port;
  }

  if (cl.arg('L').size() > 0) {
    logfile = cl.arg('L')[0];
  } else {
    logfile = "";
  }

#ifdef _DEBUG
  cerr << "Port is really set to: " << port << endl;
#endif

  string logPropFilename;
  if (cl.hasFlag('L') && cl.arg('L').size() > 0) {
    cerr << "Has L" << endl;
    istringstream os((cl.arg('L'))[0]);
    os >> logPropFilename;
  }
  QCoreApplication* app;

  if (cl.hasFlag('v')) {
    app = new QApplication(argc, argv);
  } else {
    app = new QCoreApplication(argc, argv);
  }
  server = new CoServer4(port, cl.hasFlag('d'), cl.hasFlag('v'), cl.hasFlag('L'), logPropFilename);

  if (!server->ready())
    exit(1);

  return app->exec();
}
