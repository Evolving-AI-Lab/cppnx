/*
 * CX_WeightSliderWidget.cpp
 *
 *  Created on: Jul 5, 2013
 *      Author: joost
 */

#include "CX_WeightSliderWidget.h"

#include <math.h>
#include <iostream>

#include <QLabel>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QPushButton>

#define MIN_VALUE -3.0
#define MAX_VALUE 3.0

#define MAX_SLIDER_PRECISION 2.0
#define NR_OF_TICKS 10

WeightSliderWidget::WeightSliderWidget(QWidget* object):QWidget(object) {
    resetAction = new QAction(tr("&Reset"), this);
    resetAction->setShortcut(tr("Alt+R"));
    resetAction->setStatusTip(tr("Reset all weights"));
    addAction(resetAction);
    connect(resetAction, SIGNAL(triggered()), this, SLOT(reset()));

    layout = new QHBoxLayout;

    idLabel= new QLabel("Id: ");
    idLabel->setFixedWidth(100);

    sliderBase = pow(10.0, MAX_SLIDER_PRECISION);
    sliderRange = sliderBase*(MAX_VALUE - MIN_VALUE);
    slider = new QSlider(Qt::Horizontal);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(sliderRange / NR_OF_TICKS);
    slider->setRange(0, sliderRange);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setSlider(int)));
//    connect(this, SIGNAL(sliderValueChanged(int)), slider, SLOT(setValue(int)));

    spinBox = new QDoubleSpinBox;
    spinBox->setMinimum(MIN_VALUE);
    spinBox->setMaximum(MAX_VALUE);
    spinBox->setDecimals(MAX_SLIDER_PRECISION);
    connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(setSlider(double)));

//    connect(this, SIGNAL(sliderValueChangedF(double)), spinBox, SLOT(setValue(double)));

    resetButton = new QPushButton;
    resetButton->setText(tr("reset"));
    connect(resetButton, SIGNAL(clicked()), resetAction, SLOT(trigger()));

    layout->addWidget(idLabel);
    layout->addWidget(slider);
    layout->addWidget(spinBox);
    layout->addWidget(resetButton);
    setLayout(layout);

//    this->setStyle();
}

WeightSliderWidget::~WeightSliderWidget() {
	// TODO Auto-generated destructor stub
}

void WeightSliderWidget::setEdge(double weight, double originalWeight, QString id){
	blockSignals(true);
	setSlider(weight);
	blockSignals(false);

	resetWeight = originalWeight;
	idLabel->setText("Id: " + id);
}

void WeightSliderWidget::setNode(QString id){
	idLabel->setText("Id: " + id);
}

void WeightSliderWidget::setSlider(int value){
	double weight = (float(value))*((MAX_VALUE - MIN_VALUE)/sliderRange) + MIN_VALUE;
//	spinBox->blockSignals(true);
//	spinBox->setValue(weight);
//	spinBox->blockSignals(false);
	setSlider(value, weight);
}

void WeightSliderWidget::setSlider(double weight){
	int value = round(((weight - MIN_VALUE)/(MAX_VALUE - MIN_VALUE))*sliderRange);
//	slider->blockSignals(true);
//	slider->setValue(value);
//	slider->blockSignals(false);
	setSlider(value, weight);
}

void WeightSliderWidget::setSlider(int value, double weight){
//	std::cout << value << " " << weight <<std::endl;
	if(++flashCounter > 5) flashCounter = 0;
	slider->blockSignals(true);
	slider->setValue(value);
	slider->blockSignals(false);
	spinBox->blockSignals(true);
	spinBox->setValue(weight);
	spinBox->blockSignals(false);
	emit weightChanged(weight);
}


void WeightSliderWidget::setStepSize(double stepSize){
	spinBox->setSingleStep(stepSize);
	int steps = (MAX_VALUE - MIN_VALUE) / stepSize;
	slider->setSingleStep(sliderRange/steps);
//	std::cout << stepSize <<std::endl;
//	std::cout << sliderRange << " " << steps <<std::endl;
//	std::cout << sliderRange/steps <<std::endl;
}

double WeightSliderWidget::getValue(){
	return spinBox->value();
}

bool WeightSliderWidget::flash(){
//	return slider->value()%(slider->singleStep() * 5) == 0;
	return flashCounter == 0;
}

void WeightSliderWidget::reset(){
	WeightSliderWidget::setSlider(resetWeight);
}
