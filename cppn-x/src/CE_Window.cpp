/*
 * window.cpp
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */


#include "CE_Window.h"
#include <QSpacerItem>
#include "CE_Util.h"


Window::Window()
{
	currentFileName.clear();
	cppn = 0;
	graphWidget = new GraphWidget(this);

//	graphWidget->setFixedHeight(300);

//	graphWidget->scene()->height();
//	std::cout << graphWidget->heightForWidth(200) <<std::endl;
    createMenu();


    createHorizontalGroupBox();
    horizontalGroupBox->hide();
    createGridGroupBox();

    connect(slider, SIGNAL(valueChanged(int)), graphWidget, SLOT(setValue(int)));
    connect(graphWidget, SIGNAL(sliderValueChanged(int)), slider, SLOT(setValue(int)));
    connect(graphWidget, SIGNAL(sliderValueChangedF(double)), spinBox, SLOT(setValue(double)));
    connect(spinBox, SIGNAL(valueChanged(double)), graphWidget, SLOT(setValueF(double)));

//    QGroupBox* leftGroupBox = new QGroupBox;
//    leftGroupBox->setFlat(true);
//
//    QGroupBox* rightGroupBox = new QGroupBox;
//    rightGroupBox->setFlat(true);


//    QVBoxLayout* rightLayout = new QVBoxLayout;


    sidebar = new QGraphicsView(this);
    sidebar->setMinimumSize(NodeView::node_width + sidebarMargin, NodeView::node_height + sidebarMargin);
    QGraphicsScene *scene = new QGraphicsScene(sidebar);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(0, 0, NodeView::node_width, NodeView::node_height);
    sidebar->setAlignment(Qt::AlignTop);
    sidebar->setScene(scene);


//    rightLayout->addWidget(sidebar);
//    rightGroupBox->setLayout(rightLayout);

//    colorLeftLayout = new FlowLayout();
//    colorLeftLayout->
//    colorLeftLayout->setSizeConstraint();

//	std::cout << graphWidget->heightForWidth(200) <<std::endl;

    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(graphWidget);
//    leftLayout->addWidget(sidebar);
    leftLayout->addWidget(horizontalGroupBox);
//    leftLayout->addLayout(colorLeftLayout);
//    leftLayout->addWidget(gridGroupBox);

    mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(sidebar, 0);
    mainLayout->addWidget(gridGroupBox, 0);
    mainLayout->setMenuBar(menuBar);
    setLayout(mainLayout);

    setWindowTitle(tr("Cppn-X"));
}

Window::~Window(){

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

    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit the application"));

    loadAction = new QAction(tr("&Load..."), this);
    loadAction->setShortcuts(QKeySequence::Open);
    loadAction->setStatusTip(tr("Load an existing file"));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the genome to disk"));
    saveAction->setDisabled(true);

    saveAsAction = new QAction(tr("&Save as..."), this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save the genome to disk"));
    saveAsAction->setDisabled(true);

    resetAction = new QAction(tr("&Reset"), this);
    resetAction->setShortcut(tr("Alt+R"));
    resetAction->setStatusTip(tr("Reset all weights"));
    resetAction->setDisabled(true);

    scanAction = new QAction(tr("&Scan"), this);
    scanAction->setShortcut(tr("Alt+S"));
    scanAction->setStatusTip(tr("Scan weights"));
    scanAction->setDisabled(true);

    resetAllAction = new QAction(tr("&Reset all"), this);
    resetAllAction->setStatusTip(tr("Reset all weights"));
    resetAllAction->setDisabled(true);

    circleAction = new QAction(tr("&Layers"), this);
    circleAction->setStatusTip(tr("Undo the last operation"));
    circleAction->setDisabled(true);

    layerAction = new QAction(tr("&Circle"), this);
    layerAction->setStatusTip(tr("Undo the last operation"));
    layerAction->setDisabled(true);

    addViewNodeAction = new QAction(tr("&Add to sidebar"), this);
    addViewNodeAction->setShortcut(tr("Alt+A"));
    addViewNodeAction->setStatusTip(tr("Add the selected node to the sidebar"));
    addViewNodeAction->setDisabled(true);

    deleteViewNodeAction = new QAction(tr("&Delete from sidebar"), this);
    deleteViewNodeAction->setShortcut(tr("Alt+D"));
    deleteViewNodeAction->setStatusTip(tr("Remove the selected node from the sidebar"));
    deleteViewNodeAction->setDisabled(true);

    addLabelAction = new QAction(tr("Create &label"), this);
    addLabelAction->setShortcut(tr("Alt+L"));
    addLabelAction->setStatusTip(tr("Create a new label"));
    addLabelAction->setDisabled(true);

    unlabelAction = new QAction(tr("Unlabel"), this);
    unlabelAction->setShortcut(tr("Alt+0"));
    unlabelAction->setStatusTip(tr("Remove a label from an existing object."));
    unlabelAction->setDisabled(true);

    fileMenu->addAction(exitAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    menuBar->addMenu(fileMenu);

    posMenu = new QMenu(tr("&Position"), this);
    posMenu->addAction(layerAction);
    posMenu->addAction(circleAction);

    labelMenu = new QMenu(tr("&Labels"), this);
    labelMenu->addAction(addLabelAction);
    labelMenu->addAction(unlabelAction);
    labelMenu->addSeparator();

    //Create edit menu
    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(resetAllAction);
    editMenu->addAction(resetAction);
    editMenu->addAction(scanAction);
    editMenu->addAction(addViewNodeAction);
    editMenu->addAction(deleteViewNodeAction);
    editMenu->addSeparator();
    editMenu->addMenu(posMenu);
    editMenu->addSeparator();
    editMenu->addMenu(labelMenu);
    menuBar->addMenu(editMenu);

    //Connect actions
    connect(resetAllAction, SIGNAL(triggered()), graphWidget, SLOT(resetAllWeights()));
    connect(resetAction, SIGNAL(triggered()), graphWidget, SLOT(resetWeight()));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close ()));
    connect(circleAction, SIGNAL(triggered()), graphWidget, SLOT(positionNodesLayers()));
    connect(layerAction, SIGNAL(triggered()), graphWidget, SLOT(positionNodesCircle()));
    connect(scanAction, SIGNAL(triggered()), graphWidget, SLOT(scanWeight()));
    connect(addViewNodeAction, SIGNAL(triggered()), this, SLOT(addNodeView()));
    connect(deleteViewNodeAction, SIGNAL(triggered()), this, SLOT(deleteNodeView()));
    connect(addLabelAction, SIGNAL(triggered()), this, SLOT(addColorButton()));
    connect(unlabelAction, SIGNAL(triggered()), this, SLOT(unlabel()));
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
    connect(reset, SIGNAL(clicked()), resetAction, SLOT(trigger()));

    QPushButton* scan = new QPushButton;
    scan->setText(tr("scan"));
    connect(scan, SIGNAL(clicked()), scanAction, SLOT(trigger()));

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

void Window::deleteColorButton(QWidget* object){
	CE_ColorButton* colorButton = qobject_cast<CE_ColorButton*>(object);
	if(colorButton){
		buttons.removeAll(colorButton);
		delete colorButton;

		int i=0;
		foreach(CE_ColorButton* button, buttons){
		    std::string shortcut = "Alt+" + util::toString(i+1);
		    button->getColorAction()->setShortcut(tr(shortcut.c_str()));
		    i++;
		}

		colorMainLayout->update();
		setWindowModified(true);
	}
}


void Window::addColorButton(QString text, QColor color){
    CE_ColorButton* colorButton = new CE_ColorButton(text, color);
    buttons.push_back(colorButton);
    colorMainLayout->addWidget(colorButton);

    std::string shortcut = "Alt+" + util::toString(buttons.size());
    colorButton->getColorAction()->setShortcut(tr(shortcut.c_str()));
    labelMenu->addAction(colorButton->getColorAction());

    //Map actions
    deleteSignalMapper -> setMapping (colorButton->getDeleteAction(), colorButton);
    colorSignalMapper -> setMapping (colorButton->getColorAction(), colorButton);
    connect(colorButton->getDeleteAction(), SIGNAL(triggered()), deleteSignalMapper, SLOT(map()));
    connect(colorButton->getColorAction(), SIGNAL(triggered()), colorSignalMapper, SLOT(map()));
}

void Window::addColorButton(){
	//Get label text
    bool ok = true;
    QString labelText = labelName->displayText();
    labelName->clear();
    if(labelText.isEmpty()) labelText = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("Label name:"), QLineEdit::Normal, "", &ok);
    if(!ok) return;

    //Get label color
	QColor color = QColorDialog::getColor(Qt::white, this);
    if(!color.isValid()) return;

    //Create label
    addColorButton(labelText, color);
    setWindowModified(true);
}

size_t Window::getNrOfColorButtons(){
	return buttons.size();
}

CE_ColorButton* Window::getColorButton(size_t i){
	return buttons[i];
}

void Window::createGridGroupBox()
{
	//Create 'add label' button
	addLabel = new QPushButton;
	addLabel->setText(tr("add label"));
	addLabel->setDisabled(true);

	unlabelButton = new QPushButton;
	unlabelButton->setText(tr("unlabel"));
	unlabelButton->setDisabled(true);

	//Create 'add label' text field
	labelName = new QLineEdit;
	labelName->setDisabled(true);

	//Create color layout
    colorMainLayout = new QVBoxLayout;
    colorMainLayout->setAlignment(Qt::AlignTop);
    colorMainLayout->addWidget(labelName);
    colorMainLayout->addWidget(addLabel);
    colorMainLayout->addWidget(unlabelButton);

    //Create color groupbox (should this really be a groupbox?)
    gridGroupBox = new QGroupBox(tr("Labels"));
    gridGroupBox->setLayout(colorMainLayout);


    //Create signal mappers
    deleteSignalMapper = new QSignalMapper (this) ;
    colorSignalMapper = new QSignalMapper (this) ;
    connect (deleteSignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(deleteColorButton(QWidget*))) ;
    connect (colorSignalMapper, SIGNAL(mapped(QWidget*)), graphWidget, SLOT(colorNode(QWidget*)));


    //Connect add label button
    connect(addLabel, SIGNAL(clicked()), addLabelAction, SLOT(trigger()));
    connect(labelName, SIGNAL(returnPressed()),addLabelAction,SLOT(trigger()));
    connect(unlabelButton, SIGNAL(clicked()),unlabelAction,SLOT(trigger()));
}

void Window::load(){
	QString newFileName = QFileDialog::getOpenFileName(this,
	         tr("Open Genome File"), "",
	         tr("XML File (*.xml);;All Files (*)"));
	if(newFileName.isEmpty()) return;

	try{
		if (graphWidget->load(newFileName.toStdString())){
			currentFileName = newFileName;
			addLabel->setDisabled(false);
			labelName->setDisabled(false);
			resetAllAction->setDisabled(false);
			resetAction->setDisabled(false);
			saveAction->setDisabled(false);
			saveAsAction->setDisabled(false);
			circleAction->setDisabled(false);
			layerAction->setDisabled(false);
			scanAction->setDisabled(false);
			addLabelAction->setDisabled(false);
			addViewNodeAction->setDisabled(false);
			deleteViewNodeAction->setDisabled(false);
			unlabelAction->setDisabled(false);
			unlabelButton->setDisabled(false);
		}

		clearNodeViews();
		sidebar->scene()->addItem(cppn->getFinalNodeView());
		setNodeviewPositions();

	} catch(std::exception& e){
		QString message(("Error reading file: " + newFileName.toStdString() + "\n" + std::string( e.what() )).c_str());
		QMessageBox::information(this, tr("Unable to open file"), message);
	}
}

void Window::actualSave(const QString& fileName){
	if(fileName.isEmpty()) return;
	try{
		if(graphWidget->save(fileName.toStdString())){
			cppn->setNewFile(false);
			currentFileName = fileName;
			setWindowModified(false);
		}
	} catch(std::exception& e){
		QString message(("Error saving file: " + fileName.toStdString() + "\n" + std::string( e.what() )).c_str());
		QMessageBox::information(this, tr("Unable to open file"), message);
	}
}

void Window::save(){
	if(currentFileName.isEmpty() || cppn->getNewFile()) saveAs();
	actualSave(currentFileName);
}

void Window::saveAs(){
	QString newFileName = QFileDialog::getSaveFileName(this,
	         tr("Save Genome File"), "",
	         tr("XML File (*.xml);;All Files (*)"));
	actualSave(newFileName);
}

void Window::setNodeviewPosition(NodeView* node, size_t index){
	node->setPos(NodeView::half_width,NodeView::half_height + index*(NodeView::node_height + betweenNodeMargin));
}

void Window::setSidebarSceneRect(){
	 sidebar->scene()->setSceneRect(0, 0, NodeView::node_width, sidebar->scene()->items().size()*(NodeView::node_height + betweenNodeMargin));
}


void Window::clearNodeViews(){
	sidebar->scene()->clear();
	setSidebarSceneRect();
}

void Window::setNodeviewPositions(){
	size_t index = 0;
	foreach(QGraphicsItem* item, sidebar->scene()->items()){
		NodeView* node = qgraphicsitem_cast<NodeView*> (item);
		if(node){
			setNodeviewPosition(node, index);
			index++;
		}
	}
}

void Window::deleteNodeView(NodeView* nodeToDelete){
	sidebar->scene()->removeItem(nodeToDelete);
	delete(nodeToDelete);

	setSidebarSceneRect();
	setNodeviewPositions();
}

void Window::addNodeView(Node* node){
    NodeView* testNode = new NodeView(node);
    setNodeviewPosition(testNode, sidebar->scene()->items().size());
    sidebar->scene()->addItem(testNode);
    setNodeviewPositions();
    setSidebarSceneRect();
    cppn->updateNode(node);
}

void Window::addNodeView(){
    if(graphWidget->getSelectedNode()){
    	addNodeView(graphWidget->getSelectedNode());
    }
}

void Window::deleteNodeView(){
	if(sidebar->scene()->selectedItems().count()){
		NodeView* node = qgraphicsitem_cast<NodeView*> ( sidebar->scene()->selectedItems().front());
		if(node){
			deleteNodeView(node);
		}
	}
}

void Window::closeEvent(QCloseEvent * event){
	if(isWindowModified ()){
//		 QMessageBox msgBox(tr("String"),tr("String"), QMessageBox::Question, 10, 10, 10, this, Qt::Drawer);
//		 QMessageBox msgBox(QMessageBox::Question,tr("String"),tr("String"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint |  Qt::Sheet);
		 QMessageBox msgBox(QMessageBox::Question,tr("String"),tr("String"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this, Qt::Dialog);
		 msgBox.setWindowModality ( Qt::WindowModal);

		 msgBox.setText("The file " + currentFileName +  " has been modified.");
		 msgBox.setInformativeText("Do you want to save your changes?\nNote: only position and label information will be stored, all weights will be reset.");
		 msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		 msgBox.setDefaultButton(QMessageBox::Save);
		 int ret = msgBox.exec();

		 switch (ret) {
		   case QMessageBox::Save:
		       save();
		       break;
		   case QMessageBox::Discard:
		       //Do nothing
		       break;
		   case QMessageBox::Cancel:
			   event->setAccepted(false);
		       break;
		   default:
		       // should never be reached
		       break;
		 }
	}

}

void Window::unlabel(){
	graphWidget->colorNode(Qt::white);
}
