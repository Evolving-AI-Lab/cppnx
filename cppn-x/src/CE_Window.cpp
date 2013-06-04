/*
 * window.cpp
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */


#include "CE_Window.h"
#include <QSpacerItem>


Window::Window()
{
	graphWidget = new GraphWidget(this);
    createMenu();
    QVBoxLayout *mainLayout = new QVBoxLayout;

    createHorizontalGroupBox();
    createGridGroupBox();

    connect(slider, SIGNAL(valueChanged(int)), graphWidget, SLOT(setValue(int)));
    connect(graphWidget, SIGNAL(sliderValueChanged(int)), slider, SLOT(setValue(int)));
    connect(graphWidget, SIGNAL(sliderValueChangedF(double)), spinBox, SLOT(setValue(double)));
    connect(spinBox, SIGNAL(valueChanged(double)), graphWidget, SLOT(setValueF(double)));

    mainLayout->setMenuBar(menuBar);
    mainLayout->addWidget(graphWidget);
    mainLayout->addWidget(horizontalGroupBox);
    mainLayout->addWidget(gridGroupBox);

    horizontalGroupBox->hide();
//    gridGroupBox->hide();

    setLayout(mainLayout);

    setWindowTitle(tr("Basic Layouts"));


    my_value = int(0);

}

void Window::showScrubberBar(bool visible){
	if(visible){
		horizontalGroupBox->show();
	} else {
		horizontalGroupBox->hide();
	}
}

void Window::showColorBar(bool visible){
	if(visible){
		gridGroupBox->show();
	} else {
		gridGroupBox->hide();
	}
}

void Window::createMenu()
{
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    loadAction = fileMenu->addAction(tr("&Load"));
    saveAction = fileMenu->addAction(tr("&Save"));
    menuBar->addMenu(fileMenu);

    QMenu* posMenu = new QMenu(tr("&Position"), this);
    QAction* circleAction = posMenu->addAction(tr("Layers"));
    QAction* layerAction = posMenu->addAction(tr("Circle"));
    menuBar->addMenu(posMenu);

    connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
    connect(circleAction, SIGNAL(triggered()), graphWidget, SLOT(positionNodesLayers()));
    connect(layerAction, SIGNAL(triggered()), graphWidget, SLOT(positionNodesCircle()));
}

void Window::createHorizontalGroupBox()
{
    horizontalGroupBox = new QGroupBox(tr("Modify weights"));
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

void Window::clearColorButtons(){
	while(!buttons.empty()){
		deleteColorButton(buttons.front());
	}
}

void Window::deleteColorButton(QObject* object){
	CE_ColorButton* colorButton = qobject_cast<CE_ColorButton*>(object);
	if(colorButton){
		foreach(CE_ColorButton* button, buttons){
			colorLayout->removeWidget(button->getLabel());
			colorLayout->removeWidget(button->getColorButton());
			colorLayout->removeWidget(button->getDeleteButton());
		}

		buttons.removeAll(colorButton);
		delete colorButton;

		int i=0;
		foreach(CE_ColorButton* button, buttons){
		    int row = i%2;
		    int column = int(i/2)*3;
		    colorLayout->addWidget(button->getLabel(), row, column);
		    colorLayout->addWidget(button->getColorButton(), row, column+1);
		    colorLayout->addWidget(button->getDeleteButton(), row, column+2);
		    i++;
		}

		colorLayout->update();
	}
}


void Window::addColorButton(QString text, QColor color){
    int row = buttons.size()%2;
    int column = int(buttons.size()/2)*3;

    CE_ColorButton* colorButton = new CE_ColorButton(text, color);
    buttons.push_back(colorButton);

    colorLayout->addWidget(colorButton->getLabel(), row, column);
    colorLayout->addWidget(colorButton->getColorButton(), row, column+1);
    colorLayout->addWidget(colorButton->getDeleteButton(), row, column+2);

    deleteSignalMapper -> setMapping (colorButton->getDeleteButton(), colorButton);
    colorSignalMapper -> setMapping (colorButton->getColorButton(), colorButton);

    connect(colorButton->getDeleteButton(), SIGNAL(clicked()), deleteSignalMapper, SLOT(map()));
    connect(colorButton->getColorButton(), SIGNAL(clicked()), colorSignalMapper, SLOT(map()));
}

void Window::addColorButton(){
	if(buttons.size()>=8) return;

    QColor color = QColorDialog::getColor(Qt::white, this);
    if(!color.isValid()) return;

    QLabel* label = new QLabel(labelName->displayText());
    addColorButton(labelName->displayText(), color);

    labelName->clear();

}

size_t Window::getNrOfColorButtons(){
	return buttons.size();
}

CE_ColorButton* Window::getColorButton(size_t i){
	return buttons[i];
}

void Window::createGridGroupBox()
{
    gridGroupBox = new QGroupBox(tr("Labels"));
//! [8]
    colorLayout = new QGridLayout;
    deleteSignalMapper = new QSignalMapper (this) ;
    colorSignalMapper = new QSignalMapper (this) ;
    connect (deleteSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(deleteColorButton(QObject*))) ;
    connect (colorSignalMapper, SIGNAL(mapped(QObject*)), graphWidget, SLOT(colorNode(QObject*)));

    QPushButton* addLabel = new QPushButton;
    addLabel->setText(tr("add label"));
    colorLayout->addWidget(addLabel, 1, 13);
    connect(addLabel, SIGNAL(clicked()), this, SLOT(addColorButton()));

    labelName = new QLineEdit;
    colorLayout->addWidget(labelName, 0, 13);


//! [9] //! [10]
//! [10]

//! [11]
//    QSpacerItem* space = new QSpacerItem(300,0);
//    layout->addItem(space, 0, 3);

    for(int i=0; i<12; i++){
    	colorLayout->setColumnStretch(i, 0);
    }

    colorLayout->setColumnStretch(12, 1);
    colorLayout->setColumnStretch(13, 0);

    gridGroupBox->setLayout(colorLayout);
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

void Window::save(){
	QString fileName = QFileDialog::getSaveFileName(this,
	         tr("Save Genome File"), "",
	         tr("XML File (*.xml);;All Files (*)"));
	graphWidget->save(fileName.toStdString());

}

