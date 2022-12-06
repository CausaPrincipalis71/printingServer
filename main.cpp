#include <QCoreApplication>
#include <QSettings>
#include <QFile>

#include "printingserver.h"
#include "muser.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream cerr(stderr);
    QSettings settings("MPrintingServer", "general");

    auto port = settings.value("port").toInt();
    if(port == 0)
    {
        settings.setValue("port", 1847);
        port = 1847;
    }

    printingServer MServer(port);
    if(MServer.start() == 0)
        qCritical() << "PrintingServer ERROR: Server cannot listen on port" << MServer.port();

    return a.exec();
}
