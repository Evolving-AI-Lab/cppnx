/*
 * CX_GenomeDirWidget.cpp
 *
 *  Created on: May 4, 2017
 *      Author: Joost Huizinga
 */

// Local includes
#include "CX_GenomeDirWidget.hpp"

using namespace glb_settings;

GenomeDirWidget::GenomeDirWidget(QWidget* object):
QDialog(object)
{
    this->setWindowTitle("Genome Directory");
    this->setBaseSize(800, 800);

    //Add the category title
//    QFont preferenceCategoryFont;
//    preferenceCategoryFont.setBold(true);
    QString explanation = tr("To open a CPPN genome by ID, you need access ");
    explanation += tr(" to the archive of Picbreeder genomes. You can either ");
    explanation += tr("set the genome directory to point to a local version ");
    explanation += tr("of the archive, or you can download individual genomes ");
    explanation += tr("from the archive, one at a time.\n\n");
    explanation += tr("Important: See http://picbreeder.org/legalterms.php ");
    explanation += tr("for the legal information regarding these CPPN ");
    explanation += tr("genomes.");
    _explanation = new QLabel(this);
    _explanation->setWordWrap(true);
    _explanation->setText(explanation);
//    _preferenceCategory->setFont(preferenceCategoryFont);

    //Add a separator line
    _dividerLine = new QFrame();
    _dividerLine->setFrameShape(QFrame::HLine);
    _dividerLine->setFrameShadow(QFrame::Sunken);

    //Create ok and cancel buttons
    _downloadButton =  new QPushButton(this);
    _downloadButton->setText("download genomes");
    connect(_downloadButton, SIGNAL(released()), this, SLOT(onDownload()));

    _useLocalButton =  new QPushButton(this);
    _useLocalButton->setText("use local archive");
    connect(_useLocalButton, SIGNAL(released()), this, SLOT(onUseLocal()));

    _cancelButton = new QPushButton(this);
    _cancelButton->setText("cancel");
    connect(_cancelButton, SIGNAL(released()), this, SLOT(onCancel()));

    _buttonLayout = new QHBoxLayout();
    _buttonLayout->addWidget(_downloadButton);
    _buttonLayout->addWidget(_useLocalButton);
    _buttonLayout->addWidget(_cancelButton);


    //Install the main layout
    _mainLayout = new QVBoxLayout(this);
    _mainLayout->addWidget(_explanation);
    _mainLayout->addWidget(_dividerLine);
    _mainLayout->addLayout(_buttonLayout);
}

GenomeDirWidget::~GenomeDirWidget(){
    delete _explanation;
    delete _dividerLine;
    delete _downloadButton;
    delete _useLocalButton;
    delete _cancelButton;
    delete _buttonLayout;
    delete _mainLayout;
}


void GenomeDirWidget::onCancel(){
    close();
}

void GenomeDirWidget::onUseLocal(){
    dbg::trace trace("preferences", DBG_HERE);
    if(globalSettings->getGenomeDir() == ""){
    	QString msg = tr("Select the location of the archive");
    	QString dir = QFileDialog::getExistingDirectory(this, msg);
    	if(!dir.isEmpty()){
    		globalSettings->setGenomeDir(dir);
    	} else {
    		return;
    	}
    }
    globalSettings->setDownloadPref(false);
    globalSettings->setDownloadPrefSet(true);
    close();
}

void GenomeDirWidget::onDownload(){
    dbg::trace trace("preferences", DBG_HERE);
    if(globalSettings->getGenomeDir() == ""){
    	QString msg = tr("Select where to store the downloaded genomes.");
    	QString dir = QFileDialog::getExistingDirectory(this, msg);
    	if(!dir.isEmpty()){
    		globalSettings->setGenomeDir(dir);
    	} else {
    		return;
    	}
    }
    globalSettings->setDownloadPref(true);
    globalSettings->setDownloadPrefSet(true);
    close();
}

