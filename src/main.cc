/** @file main.cc
 * Main class for coserver4. Only used for initialization.
 * Run with -gui for GUI.
 * @author Martin Lilleeng Sætra <martinls@met.no>
 */

#include "CoServer4.h"

#include <QApplication>
#include <QUrl>

#include <iostream>

#include <coserver/QLetterCommands.h>

#define MILOGGER_CATEGORY "coserver4.main"
#include <miLogger/miLogging.h>

static void helpAndExit(const char* argv0)
{
    std::cerr << argv0 << " [-d|--dynamic] [-u|--url <URL>] [-L|--log4cpp-properties-file]\n"
              << " -d enables dynamic mode\n"
              << " -u specifies socket, co4://localhost:12345/ or local:///path/to/socket\n"
              << " -L specified logging config\n"
              << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    bool dynamic = false;
    QUrl url;
    url.setScheme("co4");
    url.setHost("");
    url.setPort(qmstrings::port);
    std::string logconf;

    const QStringList args = app.arguments();
    for (int i=1; i<args.count(); ++i) {
        const QString& option = args.at(i);
        if (option == "-d" || option == "--dynamic") {
            dynamic = true;
        } else if (option == "-u" || option == "--url") {
            i += 1;
            if (i < args.count())
                url.setUrl(args.at(i));
            else
                helpAndExit(argv[0]);
        } else if (option == "-L" || option == "--log4cpp-properties-file") {
            i += 1;
            if (i < args.count())
                logconf = args.at(i).toStdString();
            else
                helpAndExit(argv[0]);
        }
    }

    milogger::LoggingConfig log4cpp(logconf);
    CoServer4 *server = new CoServer4(url, dynamic);
    if (!server->ready())
        exit(1);

    return app.exec();
}
