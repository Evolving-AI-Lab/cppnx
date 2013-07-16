/*
 * CX_WeightWidget.cpp
 *
 *  Created on: Jul 5, 2013
 *      Author: joost
 */

#include "CX_WeightWidget.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QEvent>
#include <iostream>


WeightWidget::WeightWidget(QWidget* parent): QGroupBox(parent) {
	scanning=false;
	capturing=false;
	edgeIsSelected=false;

    mainLayout = new QHBoxLayout;
    weightSlidersLayout = new QVBoxLayout;
    buttonLayout = new QHBoxLayout;

    //Create actions
    scanAction = new QAction(tr("&Start scan"), this);
    scanAction->setShortcut(tr("Alt+S"));
    scanAction->setStatusTip(tr("Scan weights"));
    connect(scanAction, SIGNAL(triggered()), this, SLOT(startScan()));
    addAction(scanAction);

    resetAllAction = new QAction(tr("&Reset all"), this);
    resetAllAction->setStatusTip(tr("Reset all weights"));
    addAction(resetAllAction);


    screenCaptureAction = new QAction(tr("Start film"), this);
    screenCaptureAction->setShortcut(tr("Alt+F"));
    screenCaptureAction->setStatusTip(tr("Capture the current screen"));
    connect(screenCaptureAction, SIGNAL(triggered()), this, SLOT(startStopFilm()));
    addAction(screenCaptureAction);

    weightSliderWidget = new WeightSliderWidget();

    resetAll = new QPushButton;
    resetAll->setText(tr("reset all"));
    connect(resetAll, SIGNAL(clicked()), resetAllAction, SLOT(trigger()));

    scan = new QPushButton;
    connect(scan, SIGNAL(clicked()), scanAction, SLOT(trigger()));

    scanAndCapture = new QPushButton;
    connect(scanAndCapture, SIGNAL(clicked()), screenCaptureAction, SLOT(trigger()));

    QLabel* stepSizeLabel = new QLabel("step size");
    QLabel* scanStartLabel = new QLabel("scan start");
    QLabel* scanStopLabel = new QLabel("scan stop");

    stepSizeSpinBox = new QDoubleSpinBox;
    stepSizeSpinBox->setMinimum(0);
    stepSizeSpinBox->setMaximum(3);
    stepSizeSpinBox->setSingleStep(0.0001);
    stepSizeSpinBox->setDecimals(4);
    connect(stepSizeSpinBox, SIGNAL(valueChanged(double)), weightSliderWidget, SLOT(setStepSize(double)));
    stepSizeSpinBox->setValue(0.10);

    scanStartSpinBox = new QDoubleSpinBox;
    scanStartSpinBox->setMinimum(-3);
    scanStartSpinBox->setMaximum(3);
    scanStartSpinBox->setValue(-3);
    scanStartSpinBox->setSingleStep(0.0001);
    scanStartSpinBox->setDecimals(4);
    connect(scanStartSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(bookendStartChanged(double)));


    scanStopSpinBox = new QDoubleSpinBox;
    scanStopSpinBox->setMinimum(-3);
    scanStopSpinBox->setMaximum(3);
    scanStopSpinBox->setValue(3);
    scanStopSpinBox->setSingleStep(0.0001);
    scanStopSpinBox->setDecimals(4);
    connect(scanStopSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(bookendEndChanged(double)));

    buttonLayout->addWidget(resetAll);
    buttonLayout->addWidget(scan);
    buttonLayout->addWidget(scanAndCapture);
    buttonLayout->addWidget(stepSizeLabel);
    buttonLayout->addWidget(stepSizeSpinBox);
    buttonLayout->addWidget(scanStartLabel);
    buttonLayout->addWidget(scanStartSpinBox);
    buttonLayout->addWidget(scanStopLabel);
    buttonLayout->addWidget(scanStopSpinBox);

    weightSlidersLayout->addWidget(weightSliderWidget);
    weightSlidersLayout->addLayout(buttonLayout);
    mainLayout->addLayout(weightSlidersLayout);

    setLayout(mainLayout);

    timerId=0;
    scanStarted(false);
    filmStarted(false);
    edgeSelected(false);
}

WeightWidget::~WeightWidget() {
	// TODO Auto-generated destructor stub
}

void WeightWidget::startStopFilm(){
	if(capturing){
		stopScan();
	} else {
		emit requestFilmStart();
	}
}

void WeightWidget::startFilm(){
	filmStarted(true);
	scanAction->setDisabled(true);
	scan->setDisabled(true);
	startScan();
	emit filmStarted();
}


void WeightWidget::startScan(){
//	std::cout << timerId <<std::endl;
	if(!timerId){
//		std::cout << "Scan started at: " << scanStartSpinBox->value() <<std::endl;
		weightSliderWidget->setSlider(scanStartSpinBox->value());
		scanStarted(true);
		emit scanStarted();
		timerId = startTimer(10);

		if(capturing){
			emit frameReady();
		} else {
		    screenCaptureAction->setDisabled(true);
		    scanAndCapture->setDisabled(true);
		}
//		block();
	} else {
		stopScan();

	}

}

void WeightWidget::stopScan(){
	killTimer(timerId);
	timerId=0;
	emit scanStopped();
	if(capturing){
		emit filmStopped();
		filmStarted(false);
	}
	scanAction->setDisabled(false);
	scan->setDisabled(false);
    screenCaptureAction->setDisabled(false);
    scanAndCapture->setDisabled(false);
	scanStarted(false);
}


void WeightWidget::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event);

//	std::cout << "Scan from: " << scanStartSpinBox->value() << " to: " << scanStopSpinBox->value() <<std::endl;
//	std::cout << "Slider at: " << weightSliderWidget->getValue() << " step size: " << stepSizeSpinBox->value() <<std::endl;
	weightSliderWidget->setSlider(weightSliderWidget->getValue() + stepSizeSpinBox->value());
	if(capturing) emit frameReady();
	if(weightSliderWidget->getValue() >= scanStopSpinBox->value()) stopScan();
}

void WeightWidget::edgeSelected(bool selected){
//	std::cout << "Edge selected: " << selected <<std::endl;
	edgeIsSelected = selected;
	scanAction->setEnabled(selected);
	screenCaptureAction->setEnabled(selected);
	scan->setEnabled(selected);
	scanAndCapture->setEnabled(selected);
	weightSliderWidget->setEnabled(selected);
	weightSliderWidget->getResetAction()->setEnabled(selected);
	stepSizeSpinBox->setEnabled(selected);
    scanStartSpinBox->setEnabled(selected);
    scanStopSpinBox->setEnabled(selected);

}

void WeightWidget::scanStarted(bool selected){
//	std::cout << "Scan started: " << selected <<std::endl;
	scanning = selected;
	stepSizeSpinBox->setDisabled(selected);
    scanStartSpinBox->setDisabled(selected);
    scanStopSpinBox->setDisabled(selected);
    weightSliderWidget->setDisabled(selected);

    if(selected){
		scanAction->setText(tr("&Stop scan"));
		scan->setText(tr("&stop scan"));
    }else {
    	scanAction->setText(tr("&Start scan"));
    	scan->setText(tr("&start scan"));
    }
}

void WeightWidget::filmStarted(bool selected){
//	std::cout << "Film started: " << selected <<std::endl;
	capturing = selected;

    if(selected){
    	screenCaptureAction->setText(tr("&Stop film"));
    	scanAndCapture->setText(tr("&stop film"));
    }else {
    	screenCaptureAction->setText(tr("&Start film"));
    	scanAndCapture->setText(tr("&start film"));
    }
}

void WeightWidget::changeEvent(QEvent* event){
	switch(event->type ()){
	case(QEvent::EnabledChange):
		foreach(QAction* action, actions()){
			action->setEnabled(isEnabled());
		}
		if(isEnabled()){
			scanStarted(scanning);
			filmStarted(capturing);
			edgeSelected(edgeIsSelected);
		}
	break;
	default:
		//Nix
		break;
	}
}

void WeightWidget::setEdge(double weight, double originalWeight, QString id, double bookendStart, double bookendEnd){
	stopScan();
//	std::cout << "Bookends: " << bookendStart << " " << bookendEnd <<std::endl;
	blockSignals(true);
	weightSliderWidget->blockSignals(true);
	weightSliderWidget->setEdge(weight, originalWeight, id);
	scanStartSpinBox->setValue(bookendStart);
	scanStopSpinBox->setValue(bookendEnd);
	weightSliderWidget->blockSignals(false);
	blockSignals(false);
}
