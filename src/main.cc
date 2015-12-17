/** @file main.cc
 * Main class for coserver4. Only used for initialization.
 * Run with -gui for GUI.
 * @author Martin Lilleeng Sætra <martinls@met.no>
 */

#include "CoServer4.h"
#include "miCommandLineStd.h"

#include <QApplication>
#include <QUrl>

#include <cstdlib>
#include <string>

#include <puTools/miStringFunctions.h>
#include <coserver/QLetterCommands.h>

#define MILOGGER_CATEGORY "coserver4.main"
#include <miLogger/miLogging.h>

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
    vector<miCommandLineStd::option> opt(5);
    std::string logfile;

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

    opt[2].flag = 'u';
    opt[2].alias = "url";
    opt[2].hasArg = true;

    opt[3].flag = 'L';
    opt[3].alias = "log4cxx-properties-file";
    opt[3].hasArg = true;

    miCommandLineStd cl(opt, argc, argv);

    QUrl url;
    if (cl.hasFlag('u') && cl.arg('u').size() >= 0) {
        url.setUrl(QString::fromStdString(cl.arg('u')[0]));
    } else {
        url.setScheme("co4");
        url.setHost("localhost");
        url.setPort(qmstrings::port);
    }

    std::auto_ptr<milogger::LoggingConfig> log4cpp;
    if (cl.hasFlag('L') && cl.arg('L').size() > 0) {
        log4cpp.reset(new milogger::LoggingConfig((cl.arg('L'))[0]));
    } else {
        log4cpp.reset(new milogger::LoggingConfig(""));
    }

    bool dynamic = cl.hasFlag('d');

    QCoreApplication app(argc, argv);
    CoServer4 *server = new CoServer4(url, dynamic);
    if (!server->ready())
        exit(1);

    return app.exec();
}
