/*
 * window.cpp
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */


#include "CE_Window.h"


Window::Window()
{
    createMenu();


    QVBoxLayout *mainLayout = new QVBoxLayout;
    graphWidget = new GraphWidget();
    createHorizontalGroupBox();

    connect(slider, SIGNAL(valueChanged(int)), graphWidget, SLOT(setValue(int)));
    connect(graphWidget, SIGNAL(sliderValueChanged(int)), slider, SLOT(setValue(int)));
    connect(graphWidget, SIGNAL(sliderValueChangedF(double)), spinBox, SLOT(setValue(double)));
    connect(spinBox, SIGNAL(valueChanged(double)), graphWidget, SLOT(setValueF(double)));

    mainLayout->setMenuBar(menuBar);
    mainLayout->addWidget(graphWidget);
    mainLayout->addWidget(horizontalGroupBox);

    setLayout(mainLayout);

    setWindowTitle(tr("Basic Layouts"));


    my_value = int(0);

}

void Window::createMenu()
{
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    loadAction = fileMenu->addAction(tr("L&oad"));
    menuBar->addMenu(fileMenu);

    connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}

void Window::createHorizontalGroupBox()
{
    horizontalGroupBox = new QGroupBox(tr("Horizontal layout"));
    QHBoxLayout *layout = new QHBoxLayout;

    slider = new QSlider(Qt::Horizontal);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(10);
    slider->setSingleStep(1);
    slider->setRange(0, 100);

    spinBox = new QDoubleSpinBox;
    spinBox->setMinimum(-3);
    spinBox->setMaximum(3);
    spinBox->setSingleStep(0.06);

    QPushButton* reset = new QPushButton;
    reset->setText(tr("reset"));
    connect(reset, SIGNAL(clicked()), graphWidget, SLOT(resetWeight()));

    QPushButton* scan = new QPushButton;
    scan->setText(tr("scan"));
    connect(scan, SIGNAL(clicked()), graphWidget, SLOT(scanWeight()));



    layout->addWidget(slider);
    layout->addWidget(spinBox);
    layout->addWidget(reset);
    layout->addWidget(scan);

    horizontalGroupBox->setLayout(layout);
}

void Window::setValue(int _value){
	my_value = _value;
	std::cout << _value << std::endl;
}

void Window::load(){
	QString fileName = QFileDialog::getOpenFileName(this,
	         tr("Open Genome File"), "",
	         tr("XML File (*.xml);;All Files (*)"));
	graphWidget->load(fileName.toStdString());

}

