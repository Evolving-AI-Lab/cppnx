//Local include
#include "CX_CppnExaminerApp.hpp"
#include "CE_ActivationFunctions.h"
#include "CX_ModuleColor.h"
#include "CX_GlobalSettings.h"

//Standard includes
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <time.h>

//Qt Includes
#include <QApplication>
#include <QtPlugin>
#include <QWidget>
#include <QtGui>
#include <QRegExp>

//Other includes
#include <zip.h>

//Q_IMPORT_PLUGIN(qjpeg)
//Q_IMPORT_PLUGIN(qgif)
//Q_IMPORT_PLUGIN(qmng)
//Q_IMPORT_PLUGIN(qico)
//Q_IMPORT_PLUGIN(qsvg)
//Q_IMPORT_PLUGIN(qtiff)


int main(int argc, char **argv) {
    time_t current_time = time(NULL);
    qsrand ( current_time );
	act_functions::initializeActivationFunctions();
	cx_colors::initModuleColors();
	glb_settings::initGlobalSettings();

	MovableObject::movableTypes.insert(Node::Type);
	MovableObject::movableTypes.insert(Legend::Type);

	CppnExaminerApp app(argc, argv);

    //Start the application
    return app.exec();
}
