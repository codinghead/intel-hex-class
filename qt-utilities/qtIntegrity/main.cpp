#include <QApplication>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);
    app.setOrganizationName("/* CODINGHEAD */");
    app.setApplicationName("qtIntegrity");

    QTranslator translator;
    translator.load("qtIntegrity-de");
    app.installTranslator(&translator);

    MainWindow mainWin;

    mainWin.show();

    return app.exec();
}
