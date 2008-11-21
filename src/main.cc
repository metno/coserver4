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

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // parsing commandline-arguments
  vector<miCommandLine::option> opt(5);

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

  miCommandLine cl(opt, qApp->argc(), qApp->argv());

  quint16 port;
  if (cl.hasFlag('p')) {
    istringstream os((cl.arg('p'))[0]);
    os >> port;
  } else {
    port = qmstrings::port;
  }

  CoServer4 *server = new CoServer4(port, cl.hasFlag('d'), cl.hasFlag('v'),
      cl.hasFlag('L'), cl.arg('L')[0]);

  if (!server->ready())
    exit(1);

  return app.exec();
}
