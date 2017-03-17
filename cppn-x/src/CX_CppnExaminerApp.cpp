/*
 * CX_CppnExaminerApp.cpp
 *
 *  Created on: Jun 3, 2015
 *      Author: Joost Huizinga
 */

// Local includes
#include "CX_CppnExaminerApp.hpp"
#include "CX_GlobalSettings.h"


CppnExaminerApp::CppnExaminerApp( int & argc, char **argv )
    : QApplication(argc, argv){
    //Parse command line options
    ArgumentParser argumentParser;
    argumentParser.setVersion("CPPN-X Version: 0.8.0");

    argumentParser.addOption("-v", "--verbose");
    argumentParser.getLastOption()->setMinNbOfArguments(1);
    argumentParser.getLastOption()->setMaxNbOfArguments(Option::infinite);
    argumentParser.getLastOption()->setHelp("writes additional debug information to the shell depending on the flags provided");

    argumentParser.addOption("-t", "--trace");
    argumentParser.getLastOption()->setMaxNbOfArguments(0);
    argumentParser.getLastOption()->setHelp("enables tracing. Requires that one or more --verbose flags are set.");

    argumentParser.addPositionalOption("input");
    argumentParser.getLastOption()->setMaxNbOfArguments(1);
    argumentParser.getLastOption()->setHelp("file to be loaded upon open.");

    QMap<QString, QStringList> options = argumentParser.parse(arguments());

    //Initialize the debugger
    dbg::init();
    attach_ostream(dbg::warning, std::cout);
    attach_ostream(dbg::error, std::cerr);
    attach_ostream(dbg::info, std::cout);

    //Process the basic options 'help' and 'verbose'
    if (options.count("verbose")){
        QStringList streams = options["verbose"];
        bool trace = options.count("trace") >= 1;
        foreach(QString stream, streams){
            std::string string = stream.toStdString();
            dbg::enable(dbg::all, string.c_str(), true);
            dbg::attach_ostream(dbg::info, string.c_str(), std::cout);
            if (trace){
                dbg::attach_ostream(dbg::tracing, string.c_str(), std::cout);
            }
        }

        if (trace){
            attach_ostream(dbg::tracing, std::cout);
        }
        dbg::out(dbg::info, "debug") << "Debugger is now active" << std::endl;
    }

    _mainWindow = new Window();
    _mainWindow->show();

    if(options.count("input")){
        _mainWindow->loadFile(options["input"].front());
    }
}

CppnExaminerApp::~CppnExaminerApp()
{
    delete _mainWindow;
    glb_settings::uninitGlobalSettings();
}

void CppnExaminerApp::loadFile(const QString &fileName)
{
    _mainWindow->loadFile(fileName);

}

bool CppnExaminerApp::event(QEvent *event)
{
        switch (event->type()) {
        case QEvent::FileOpen:
            loadFile(static_cast<QFileOpenEvent *>(event)->file());
            return true;
        default:
            return QApplication::event(event);
    }
}
