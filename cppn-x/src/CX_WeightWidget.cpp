/*
 * CX_WeightWidget.cpp
 *
 *  Created on: Jul 5, 2013
 *      Author: joost
 */

//Standard includes
#include <iostream>

//QT includes
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QEvent>

//Local includes
#include "CX_WeightWidget.h"

WeightWidget::WeightWidget(QWidget* parent): QGroupBox(parent) {
	scanning=false;
	capturing=false;
	edgeIsSelected=false;

    mainLayout = new QHBoxLayout;
    weightSlidersLayout = new QVBoxLayout;
    buttonLayout = new QHBoxLayout;

    stepLayout = new QVBoxLayout;
    startLayout = new QVBoxLayout;
    endLayout = new QVBoxLayout;
    resetStartHereLayout = new QVBoxLayout;
    scanFilmLayout = new QVBoxLayout;

    //Create actions
    scanAction = new QAction(tr("&Start scan"), this);
    scanAction->setShortcut(tr("Alt+S"));
    scanAction->setStatusTip(tr("Scan weights"));
    connect(scanAction, SIGNAL(triggered()), this, SLOT(startScan()));
    addAction(scanAction);

    resetAllAction = new QAction(tr("&Reset all"), this);
    resetAllAction->setShortcut(tr("Alt+Shift+R"));
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

    stepSizeLabel = new QLabel("step");
    scanStartLabel = new QLabel("start");
    scanStopLabel = new QLabel("end");

    stepSizeLabel->setAlignment(Qt::AlignCenter);
    scanStartLabel->setAlignment(Qt::AlignCenter);
    scanStopLabel->setAlignment(Qt::AlignCenter);

    startHereButton = new QPushButton(tr("start here"));
    startHereButton->setCheckable(true);



    stepSizeSpinBox = new QDoubleSpinBox;
    stepSizeSpinBox->setMinimum(0);
    stepSizeSpinBox->setMaximum(3);
    stepSizeSpinBox->setSingleStep(0.01);
    stepSizeSpinBox->setDecimals(2);
    connect(stepSizeSpinBox, SIGNAL(valueChanged(double)), weightSliderWidget, SLOT(setStepSize(double)));
    connect(stepSizeSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(bookendStepChanged(double)));
    stepSizeSpinBox->setValue(0.10);

    scanStartSpinBox = new QDoubleSpinBox;
    scanStartSpinBox->setMinimum(-3);
    scanStartSpinBox->setMaximum(3);
    scanStartSpinBox->setValue(-3);
    scanStartSpinBox->setSingleStep(0.01);
    scanStartSpinBox->setDecimals(2);
    connect(scanStartSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(bookendStartChanged(double)));


    scanStopSpinBox = new QDoubleSpinBox;
    scanStopSpinBox->setMinimum(-3);
    scanStopSpinBox->setMaximum(3);
    scanStopSpinBox->setValue(3);
    scanStopSpinBox->setSingleStep(0.01);
    scanStopSpinBox->setDecimals(2);
    connect(scanStopSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(bookendEndChanged(double)));

    resetStartHereLayout->addWidget(resetAll);
    resetStartHereLayout->addWidget(startHereButton);
    scanFilmLayout->addWidget(scan);
    scanFilmLayout->addWidget(scanAndCapture);

    stepLayout->addWidget(stepSizeLabel);
    stepLayout->addWidget(stepSizeSpinBox);
    startLayout->addWidget(scanStartLabel);
    startLayout->addWidget(scanStartSpinBox);
    endLayout->addWidget(scanStopLabel);
    endLayout->addWidget(scanStopSpinBox);

    buttonLayout->addLayout(resetStartHereLayout);
    buttonLayout->addLayout(scanFilmLayout);
    buttonLayout->addLayout(stepLayout);
    buttonLayout->addLayout(startLayout);
    buttonLayout->addLayout(endLayout);

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

}

void WeightWidget::setValue(double value){
    stopScan();
    weightSliderWidget->blockSignals(true);
    weightSliderWidget->setSlider(value);
    weightSliderWidget->blockSignals(false);
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
		if(startHereButton->isChecked()){
			scanState = 1;
			start = weightSliderWidget->getValue();
		} else {
			scanState = 0;
			weightSliderWidget->setSlider(scanStartSpinBox->value());
		}

		weightSliderWidget->resetFlash();


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
	emit flash(false);
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

	if(scanState == 0 || scanState == 2){
		weightSliderWidget->setSlider(weightSliderWidget->getValue() + stepSizeSpinBox->value());
	} else {
		weightSliderWidget->setSlider(weightSliderWidget->getValue() - stepSizeSpinBox->value());
	}

	if(weightSliderWidget->flash()) emit flash(true);
	if(capturing) emit frameReady();

	switch(scanState){
	case 0:
		if(weightSliderWidget->getValue() >= scanStopSpinBox->value()) stopScan();
	break;
	case 1:
		if(weightSliderWidget->getValue() <= scanStartSpinBox->value()) scanState++;
	break;
	case 2:
		if(weightSliderWidget->getValue() >= scanStopSpinBox->value()) scanState++;
	break;
	case 3:
		if(weightSliderWidget->getValue() <= start){
			stopScan();
			weightSliderWidget->setSlider(start);
		}
	break;
	default:

	break;
	}

}

void WeightWidget::edgeSelected(bool selected){
//	std::cout << "Edge selected: " << selected <<std::endl;
	weightSliderWidget->setNode(tr(""));
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

void WeightWidget::setEdge(double weight, double originalWeight, QString id, double bookendStart, double bookendEnd, double bookendStep){
	stopScan();
//	std::cout << "Bookends: " << bookendStart << " " << bookendEnd <<std::endl;
	blockSignals(true);
	weightSliderWidget->blockSignals(true);
	weightSliderWidget->setEdge(weight, originalWeight, id);
	scanStartSpinBox->setValue(bookendStart);
	scanStopSpinBox->setValue(bookendEnd);
	stepSizeSpinBox->setValue(bookendStep);
	weightSliderWidget->blockSignals(false);
	blockSignals(false);
}

void WeightWidget::setNode(QString id){
//	stopScan();
//	std::cout << "Bookends: " << bookendStart << " " << bookendEnd <<std::endl;
	weightSliderWidget->setNode(id);
}
