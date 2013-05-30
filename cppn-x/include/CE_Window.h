/*
 * window.h
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <QDialog>
#include "CE_CppnWidget.h"
#include <QtGui>
#include <iostream>


QT_BEGIN_NAMESPACE
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE

//! [0]
class Window : public QDialog
{
    Q_OBJECT

public slots:
    void setValue(int _value);
    void load();

public:
    Window();
//    ~Window();

private:
    void createMenu();
    void createHorizontalGroupBox();

    QMenuBar *menuBar;
    QGroupBox *horizontalGroupBox;
    QMenu *fileMenu;
    QAction *exitAction;
    QAction *loadAction;

    GraphWidget *graphWidget;
    QSlider* slider;
    QDoubleSpinBox *spinBox;

    int my_value;
};
//! [0]


#endif /* WINDOW_H_ */
