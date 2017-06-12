/*
 * CX_PreferencesWidget.hpp
 *
 *  Created on: Jun 9, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_PREFERENCESWIDGET_HPP_
#define CPPN_X_INCLUDE_CX_PREFERENCESWIDGET_HPP_

// QT Includes
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtGui>

// Local includes
#include "CX_GlobalSettings.h"

class PreferencesWidget: public QDialog{
    Q_OBJECT
public:
    PreferencesWidget(QWidget* object = 0);
    virtual ~PreferencesWidget();

public slots:
    void onCancel();
    void onOk();
    void onChangeGenomeDir();

private:

    // Private variables regarding the layout of this widget
    QLabel* _preferenceCategory;
    QFrame* _dividerLine;

    QLineEdit* _genomeDirectoryLineEdit;
    QLabel* _genomeDirectoryLabel;
    QPushButton* _genomeDirectoryButton;
    QHBoxLayout* _genomeDirectoryLayout;

    QFrame* _dividerLine2;
    QCheckBox* _downloadCheckbox;
    QFrame* _dividerLine3;
    QPushButton* _okButton;
    QPushButton* _cancelButton;
    QHBoxLayout* _okCancelLayout;

    QVBoxLayout* _mainLayout;
};



#endif /* CPPN_X_INCLUDE_CX_PREFERENCESWIDGET_HPP_ */
