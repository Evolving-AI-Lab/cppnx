/*
 * CX_GenomeDirWidget.hpp
 *
 *  Created on: May 4, 2017
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_GENOMEDIRWIDGET_HPP_
#define CPPN_X_INCLUDE_CX_GENOMEDIRWIDGET_HPP_

// QT Includes
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtGui>

// Local includes
#include "CX_GlobalSettings.h"

class GenomeDirWidget: public QDialog{
    Q_OBJECT
public:
	GenomeDirWidget(QWidget* object = 0);
    virtual ~GenomeDirWidget();

public slots:
    void onCancel();
    void onUseLocal();
    void onDownload();

private:

    // Private variables regarding the layout of this widget
    QLabel* _explanation;
    QFrame* _dividerLine;

    QPushButton* _downloadButton;
    QPushButton* _useLocalButton;
    QPushButton* _cancelButton;
    QHBoxLayout* _buttonLayout;

    QVBoxLayout* _mainLayout;
};



#endif /* CPPN_X_INCLUDE_CX_GENOMEDIRWIDGET_HPP_ */
