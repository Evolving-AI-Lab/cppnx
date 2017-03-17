/*
 * CX_PreferencesWidget.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: Joost Huizinga
 */

// Local includes
#include "CX_PreferencesWidget.hpp"

using namespace glb_settings;

PreferencesWidget::PreferencesWidget(QWidget* object):
QDialog(object)
{
    this->setWindowTitle("Preferences");
    //this->setBaseSize(800, 800);

    //Add the category title
    QFont preferenceCategoryFont;
    preferenceCategoryFont.setBold(true);
    _preferenceCategory = new QLabel(this);
    _preferenceCategory->setText("General");
    _preferenceCategory->setFont(preferenceCategoryFont);

    //Add a separator line
    _dividerLine = new QFrame();
    _dividerLine->setFrameShape(QFrame::HLine);
    _dividerLine->setFrameShadow(QFrame::Sunken);

    //Set the genome directory line
    _genomeDirectoryLabel = new QLabel(this);
    _genomeDirectoryLabel->setText("Genome directory:");
    _genomeDirectoryLineEdit = new QLineEdit(this);
    _genomeDirectoryLineEdit->setReadOnly(true);
    _genomeDirectoryLineEdit->setText(globalSettings->getGenomeDir());
    _genomeDirectoryLineEdit->setMinimumWidth(300);
    _genomeDirectoryButton = new QPushButton(this);
    _genomeDirectoryButton->setText("change");
    connect(_genomeDirectoryButton, SIGNAL(released()), this, SLOT(onChangeGenomeDir()));
    _genomeDirectoryLayout = new QHBoxLayout();
    _genomeDirectoryLayout->addWidget(_genomeDirectoryLabel);
    _genomeDirectoryLayout->addWidget(_genomeDirectoryLineEdit);
    _genomeDirectoryLayout->addWidget(_genomeDirectoryButton);

    //Add another separator line
    _dividerLine2 = new QFrame();
    _dividerLine2->setFrameShape(QFrame::HLine);
    _dividerLine2->setFrameShadow(QFrame::Sunken);

    //Create ok and cancel buttons
    _okButton =  new QPushButton(this);
    _okButton->setText("ok");
    connect(_okButton, SIGNAL(released()), this, SLOT(onOk()));

    _cancelButton = new QPushButton(this);
    _cancelButton->setText("cancel");
    connect(_cancelButton, SIGNAL(released()), this, SLOT(onCancel()));

    _okCancelLayout = new QHBoxLayout();
    _okCancelLayout->addWidget(_cancelButton);
    _okCancelLayout->addWidget(_okButton);

    //Install the main layout
    _mainLayout = new QVBoxLayout(this);
    _mainLayout->addWidget(_preferenceCategory);
    _mainLayout->addWidget(_dividerLine);
    _mainLayout->addLayout(_genomeDirectoryLayout);
    _mainLayout->addWidget(_dividerLine2);
    _mainLayout->addLayout(_okCancelLayout);
}

PreferencesWidget::~PreferencesWidget(){
    delete _preferenceCategory;
    delete _dividerLine;
    delete _genomeDirectoryLabel;
    delete _genomeDirectoryLineEdit;
    delete _genomeDirectoryButton;
    delete _genomeDirectoryLayout;
    delete _dividerLine2;
    delete _okButton;
    delete _cancelButton;
    delete _okCancelLayout;
    delete _mainLayout;
}


void PreferencesWidget::onCancel(){
    close();
}

void PreferencesWidget::onOk(){
    globalSettings->setGenomeDir(_genomeDirectoryLineEdit->text());
    globalSettings->save();
    close();
}

void PreferencesWidget::onChangeGenomeDir(){
    dbg::trace trace("preferences", DBG_HERE);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select genome directory"));
    if(!dir.isEmpty()){
        _genomeDirectoryLineEdit->setText(dir);
    }
    update();
}

