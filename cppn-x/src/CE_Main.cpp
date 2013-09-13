#include "CE_Window.h"
#include "CE_ActivationFunctions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <QApplication>
#include <QtPlugin>
#include <QWidget>
#include <QtGui>

#include <zip.h>

//Q_IMPORT_PLUGIN(qjpeg)
//Q_IMPORT_PLUGIN(qgif)
//Q_IMPORT_PLUGIN(qmng)
//Q_IMPORT_PLUGIN(qico)
//Q_IMPORT_PLUGIN(qsvg)
//Q_IMPORT_PLUGIN(qtiff)


int main(int argc, char **argv) {
	act_functions::initializeActivationFunctions();
    QApplication app(argc, argv);
    Window dialog;

    dialog.show();


    return app.exec();
}
