/*
 * CX_CppnExaminerApp.hpp
 *
 *  Created on: Jun 3, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_CPPNEXAMINERAPP_HPP_
#define CPPN_X_INCLUDE_CX_CPPNEXAMINERAPP_HPP_

//Local includes
#include "CE_Window.h"
#include "CX_ArgumentParser.h"
#include "CX_Debug.hpp"

//Qt Includes
#include <QApplication>
#include <QtGui>

class CppnExaminerApp : public QApplication
{
    Q_OBJECT
public:
    CppnExaminerApp(int & argc, char **argv);
    virtual ~CppnExaminerApp();
protected:
    bool event(QEvent *);
private:
    void loadFile(const QString &fileName);

    Window *_mainWindow;
};

#endif /* CPPN_X_INCLUDE_CX_CPPNEXAMINERAPP_HPP_ */
