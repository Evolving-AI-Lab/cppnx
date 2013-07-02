/*
 * window.cpp
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */



#include <QSpacerItem>

#include "CE_Window.h"
#include "CE_Util.h"
#include "CE_LabelWidget.h"

#include "CE_NodeView.h"
#include "CE_Node.h"

#include "CE_CommandLabelObject.h"
#include "CE_CommandSetWeight.h"
#include "CE_CommandAddLabel.h"
#include "CE_CommandNodeView.h"



Window::Window()
{
    edgeIsSelected = false;
    selectedEdge = NULL;
    scannedEdge = NULL;
    nodeIsSelected = false;
    labelMode = onlyLabels;
    updateEdges =true;

	currentFileName.clear();
	cppn = 0;
	graphWidget = new GraphWidget(this);
    connect(graphWidget->scene(), SIGNAL(selectionChanged()), this, SLOT(updateMainSelection()));

//	graphWidget->setFixedHeight(300);

//	graphWidget->scene()->height();
//	std::cout << graphWidget->heightForWidth(200) <<std::endl;
    createMenu();
    createWeightBar();
    createLabelBar();
    createNodeViewBar();




//    QGroupBox* leftGroupBox = new QGroupBox;
//    leftGroupBox->setFlat(true);
//
//    QGroupBox* rightGroupBox = new QGroupBox;
//    rightGroupBox->setFlat(true);


//    QVBoxLayout* rightLayout = new QVBoxLayout;







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
    mainLayout->addLayout(colorMainLayout, 0);
//    mainLayout->addLayout(colorMainLayout, 0);
    mainLayout->setMenuBar(menuBar);
    setLayout(mainLayout);

    setWindowTitle(tr("Untitled"));
    capture=false;
    timerId=0;


}

Window::~Window(){

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

    screenCaptureAction = new QAction(tr("Film"), this);
    screenCaptureAction->setShortcut(tr("Alt+F"));
    screenCaptureAction->setStatusTip(tr("Capture the current screen"));
    screenCaptureAction->setDisabled(true);

    labelOnlyAction = new QAction(tr("Label only"), this);
    labelOnlyAction->setStatusTip(tr("Capture the current screen"));

    signOnlyAction = new QAction(tr("Sign only"), this);
    signOnlyAction->setStatusTip(tr("Capture the current screen"));

    labelAndSignAction = new QAction(tr("Sign and label"), this);
    labelAndSignAction->setStatusTip(tr("Capture the current screen"));

    undoAction = undoStack.createUndoAction(this, tr("Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    redoAction = undoStack.createRedoAction(this, tr("Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

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
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addAction(resetAllAction);
    editMenu->addAction(resetAction);
    editMenu->addAction(scanAction);
    editMenu->addAction(addViewNodeAction);
    editMenu->addAction(deleteViewNodeAction);
    editMenu->addAction(screenCaptureAction);
    editMenu->addSeparator();
    editMenu->addMenu(posMenu);
    editMenu->addSeparator();
    editMenu->addMenu(labelMenu);
    menuBar->addMenu(editMenu);

    viewMenu= new QMenu(tr("&View"), this);
    viewMenu->addAction(labelOnlyAction);
    viewMenu->addAction(signOnlyAction);
    viewMenu->addAction(labelAndSignAction);
    menuBar->addMenu(viewMenu);

    //Connect actions
    connect(resetAllAction, SIGNAL(triggered()), this, SLOT(resetAllWeights()));
    connect(resetAction, SIGNAL(triggered()), this, SLOT(resetWeight()));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close ()));
    connect(circleAction, SIGNAL(triggered()), graphWidget, SLOT(positionNodesLayers()));
    connect(layerAction, SIGNAL(triggered()), graphWidget, SLOT(positionNodesCircle()));
    connect(scanAction, SIGNAL(triggered()), this, SLOT(startScan()));
    connect(addViewNodeAction, SIGNAL(triggered()), this, SLOT(addNodeView()));
    connect(deleteViewNodeAction, SIGNAL(triggered()), this, SLOT(deleteNodeView()));
    connect(addLabelAction, SIGNAL(triggered()), this, SLOT(addColorButton()));
    connect(unlabelAction, SIGNAL(triggered()), this, SLOT(unlabel()));
    connect(screenCaptureAction, SIGNAL(triggered()), this, SLOT(captureScreen()));


    connect(labelOnlyAction, SIGNAL(triggered()), this, SLOT(setLabelOny()));
    connect(signOnlyAction, SIGNAL(triggered()), this, SLOT(setSignOny()));
    connect(labelAndSignAction, SIGNAL(triggered()), this, SLOT(setBoth()));
}

void Window::createNodeViewBar()
{
    sidebar = new QGraphicsView(this);
    sidebar->setMinimumSize(NodeView::node_width + sidebarMargin, NodeView::node_height + sidebarMargin);
    QGraphicsScene *scene = new QGraphicsScene(sidebar);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(0, 0, NodeView::node_width, NodeView::node_height);
    sidebar->setAlignment(Qt::AlignTop);
    sidebar->setScene(scene);
    connect(sidebar->scene(), SIGNAL(selectionChanged()), this, SLOT(updateSidebarSelection()));
}

void Window::createWeightBar()
{
    horizontalGroupBox = new QGroupBox(tr("Modify weights"));
    QHBoxLayout *layout = new QHBoxLayout;

    slider = new QSlider(Qt::Horizontal);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(10);
    slider->setSingleStep(1);
    slider->setRange(0, 100);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    connect(this, SIGNAL(sliderValueChanged(int)), slider, SLOT(setValue(int)));

    spinBox = new QDoubleSpinBox;
    spinBox->setMinimum(-3);
    spinBox->setMaximum(3);
    spinBox->setSingleStep(0.06);
    connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(setValueF(double)));
    connect(this, SIGNAL(sliderValueChangedF(double)), spinBox, SLOT(setValue(double)));

    QPushButton* reset = new QPushButton;
    reset->setText(tr("reset"));
    connect(reset, SIGNAL(clicked()), resetAction, SLOT(trigger()));

    QPushButton* resetAll = new QPushButton;
    resetAll->setText(tr("reset all"));
    connect(resetAll, SIGNAL(clicked()), resetAllAction, SLOT(trigger()));

    QPushButton* scan = new QPushButton;
    scan->setText(tr("scan"));
    connect(scan, SIGNAL(clicked()), scanAction, SLOT(trigger()));

    QPushButton* scanAndCapture = new QPushButton;
    scanAndCapture->setText(tr("film"));
    connect(scanAndCapture, SIGNAL(clicked()), screenCaptureAction, SLOT(trigger()));

    layout->addWidget(slider);
    layout->addWidget(spinBox);
    layout->addWidget(reset);
    layout->addWidget(resetAll);
    layout->addWidget(scan);
    layout->addWidget(scanAndCapture);


    horizontalGroupBox->setDisabled(true);
    horizontalGroupBox->setLayout(layout);


}

void Window::createLabelBar()
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




    colorLabelLayout = new QVBoxLayout();
    colorLabelLayout->setAlignment(Qt::AlignTop);
//    colorLabelLayout->setMargin(0);
    colorLabelLayout->setSpacing(1);
//    colorMainLayout->setSizeConstraint(QVBoxLayout::SetMinimumSize);

//    colorLabelWidget = new QWidget();
//    colorLabelWidget->setLayout(colorLabelLayout);


    QPushButton* test = new QPushButton;
    test->setText(tr("add label"));

	labelBar = new VerticalScrollArea();
	labelBar->m_scrollAreaWidgetContents->setLayout(colorLabelLayout);


	//Create color layout
    colorMainLayout = new QVBoxLayout();
    colorMainLayout->setAlignment(Qt::AlignTop);
    colorMainLayout->addWidget(labelName);
    colorMainLayout->addWidget(addLabel);
    colorMainLayout->addWidget(unlabelButton);
    colorMainLayout->addWidget(labelBar);

    //Create color groupbox (should this really be a groupbox?)

//	labelBar->setLayout(colorMainLayout);

//	labelBar->setWidget(gridGroupBox);

//    gridGroupBox = new QGroupBox(tr("Labels"));
//    gridGroupBox->setLayout(colorMainLayout);




    //Create signal mappers
    deleteSignalMapper = new QSignalMapper (this) ;
    colorSignalMapper = new QSignalMapper (this) ;
    connect (deleteSignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(deleteColorButton(QWidget*))) ;
    connect (colorSignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(colorNode(QWidget*)));


    //Connect add label button
    connect(addLabel, SIGNAL(clicked()), addLabelAction, SLOT(trigger()));
    connect(labelName, SIGNAL(returnPressed()),addLabelAction,SLOT(trigger()));
    connect(unlabelButton, SIGNAL(clicked()),unlabelAction,SLOT(trigger()));


}





void Window::clearColorButtons(){
//	std::cout << "Nr of labelWidgets: " << colorLabelLayout->count() <<std::endl;
	QList<LabelWidget*> toDelete;
	for(int i=0; i<colorLabelLayout->count(); i++){
		if (LabelWidget *label = qobject_cast<LabelWidget*>(dynamic_cast <QWidgetItem*>(colorLabelLayout->itemAt(i))->widget())) {
			toDelete.append(label);
		}
	}

	foreach(LabelWidget *label, toDelete){
		removeLabelWidget(label);
	}
}

void Window::deleteColorButton(QWidget* object){
	LabelWidget* labelWidget = qobject_cast<LabelWidget*>(object);
	if(labelWidget) {
//		std::cout << "Delete button" <<std::endl;
		undoStack.push(new CommandAddLabel(this, labelWidget, false));
	}
}

void Window::addColorButton(){
//	std::cout << "addColorButton" <<std::endl;

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
    undoStack.push(new CommandAddLabel(this, new LabelWidget(this, labelText, color, false), true));
    setWindowModified(true);
}


//void Window::addColorButton(QString text, QColor color){
//    LabelWidget* colorButton = new LabelWidget(text, color, false);
//    addLabelWidget(colorButton);
//}

void Window::addLabelWidget(LabelWidget* labelWidget, size_t index){
//	std::cout << "addLabelWidget" <<std::endl;
	labelWidget->registerObject();
	if(labelWidget->getId() == 0){
		colorLabelLayout->addWidget(labelWidget);
		labelWidget->setId(colorLabelLayout->count());
	} else {
		colorLabelLayout->insertWidget(labelWidget->getId()-1, labelWidget);
	}

    labelMenu->addAction(labelWidget->getColorAction());

    //Map actions

    deleteSignalMapper -> setMapping (labelWidget->getDeleteAction(), labelWidget);
    colorSignalMapper -> setMapping (labelWidget->getColorAction(), labelWidget);
    connect(labelWidget->getDeleteAction(), SIGNAL(triggered()), deleteSignalMapper, SLOT(map()));
    connect(labelWidget->getColorAction(), SIGNAL(triggered()), colorSignalMapper, SLOT(map()));
    graphWidget->updateAll();
}

void Window::removeLabelWidget(LabelWidget* labelWidget){
	labelWidget->setDeleted();
    disconnect(labelWidget->getDeleteAction(), SIGNAL(triggered()), deleteSignalMapper, SLOT(map()));
    disconnect(labelWidget->getColorAction(), SIGNAL(triggered()), colorSignalMapper, SLOT(map()));
	colorLabelLayout->removeWidget(labelWidget);
	labelMenu->removeAction(labelWidget->getColorAction());
	labelWidget->unregisterObject();


//	std::cout << labelWidget->registerdObjects <<std::endl;


	id_t index=1;
	for(int i=0; i<colorLabelLayout->count(); i++){
		if (LabelWidget *label = qobject_cast<LabelWidget*>(dynamic_cast <QWidgetItem*>(colorLabelLayout->itemAt(i))->widget())) {
			label->setId(index);
			index++;
		}
	}

	colorLabelLayout->update();
	graphWidget->updateAll();
}



size_t Window::getNrOfColorButtons(){
	return colorLabelLayout->count();
}

LabelWidget* Window::getColorButton(size_t i){
	return qobject_cast<LabelWidget*>(dynamic_cast <QWidgetItem*>(colorLabelLayout->itemAt(i))->widget());
}



void Window::load(){
	QString newFileName = QFileDialog::getOpenFileName(this,
	         tr("Open Genome File"), "",
	         tr("Genome file (*.xml;*.zip);;All Files (*)"));
	if(newFileName.isEmpty()) return;

	try{

		graphWidget->scene()->clear();
		undoStack.clear();
		delete cppn;
		clearColorButtons();


		graphWidget->load(newFileName.toStdString());
		currentFileName = newFileName;
		addLabel->setDisabled(false);
		labelName->setDisabled(false);
		resetAllAction->setDisabled(false);
		saveAction->setDisabled(false);
		saveAsAction->setDisabled(false);
		circleAction->setDisabled(false);
		layerAction->setDisabled(false);
		addLabelAction->setDisabled(false);


		clearNodeViews();
		sidebar->scene()->addItem(cppn->getFinalNodeView());
		setNodeviewPositions();
		setWindowTitle(newFileName.split('/').back());


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
		NodeView* node = util::multiCast<NodeView*, FinalNodeView*> (item);
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
    setSidebarSceneRect();
    cppn->updateNode(node);
}

void Window::addNodeView(){
	undoStack.push(new CommandNodeView(sidebar, graphWidget->scene()->selectedItems(), true));
//	foreach(QGraphicsItem* item, graphWidget->scene()->selectedItems()){
//		Node* node = qgraphicsitem_cast<Node*> (item);
//		if(node) addNodeView(node);
//	}
}

void Window::deleteNodeView(){
	undoStack.push(new CommandNodeView(sidebar, sidebar->scene()->selectedItems(), false));
//	foreach(QGraphicsItem* item, sidebar->scene()->selectedItems()){
//		NodeView* node = qgraphicsitem_cast<NodeView*> (item);
//		if(node) deleteNodeView(node);
//	}
}

void Window::closeEvent(QCloseEvent * event){
	if(isWindowModified ()){
//		 QMessageBox msgBox(tr("String"),tr("String"), QMessageBox::Question, 10, 10, 10, this, Qt::Drawer);
//		 QMessageBox msgBox(QMessageBox::Question,tr("String"),tr("String"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint |  Qt::Sheet);
		 QMessageBox msgBox(QMessageBox::Question,tr("Save as"),"The file " + currentFileName +  " has been modified.", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this, Qt::Dialog);
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



void Window::captureScreen(){
	captureDirectory = QFileDialog::getExistingDirectory(this, tr("Chose save directory"), "", QFileDialog::ShowDirsOnly);
	if(captureDirectory == "") return;

	nodeViewsToBeCaptured.clear();
	capture=true;
//	captureDirectory = QFileDialog::getSaveFileName(this, tr("Chose save directory"), "");


	this->setFixedSize(this->width(),this->height());
	QDir().mkdir(captureDirectory + "/fullApplication");

#ifdef USE_FFMPEG
	nodeViewEncoders.clear();
	encoder.createFile(captureDirectory + "/fullApplication.mpg",this->width(),this->height(),this->width()*this->height()*4,10);
#endif USE_FFMPEG

	QList<QGraphicsItem*> graphicsItemsToBeEncoded = sidebar->scene()->selectedItems();
	foreach(QGraphicsItem* item, graphicsItemsToBeEncoded){
		NodeView* nodeView = util::multiCast<NodeView*, FinalNodeView*>(item);
		if(nodeView){
			nodeViewsToBeCaptured.append(nodeView);
			QDir().mkdir(captureDirectory + "/node" + util::toQString(nodeViewsToBeCaptured.size()));

#ifdef USE_FFMPEG
			QVideoEncoder* nodeViewEncoder = new QVideoEncoder();
			nodeViewEncoder->createFile(captureDirectory + "/node" + util::toQString(nodeViewsToBeCaptured.size()) + ".mpg",256,256,262144,10);
			nodeViewEncoders.append(nodeViewEncoder);

//			encoder.createFile(captureDirectory + "/node" + util::toQString(nodeViewsToBeCaptured.size()) + ".mpg",256,256,18000,10);
#endif USE_FFMPEG
		}
	}

	startScan();
	captureFrame();

}

void Window::startScan(){
//	std::cout << timerId <<std::endl;
	if(!timerId){
		scannedEdge = selectedEdge;
		if(scannedEdge){
			timerId = startTimer(10);
			slider->setValue(0);
		}
	} else {
		stopScan();
	}
}

void Window::captureFrame(){
	//		QPixmap test = QPixmap::grabWindow(this->winId(), 0, 0, 1024, 768);
	QPixmap test = QPixmap::grabWidget(this, 0, 0, this->width(), this->height());
	QString name = captureDirectory + "/fullApplication/frame%1.jpg";
	name = name.arg(util::toQString(slider->value()), 4, '0');
	test.save(name);

#ifdef USE_FFMPEG
	QImage image = test.toImage();
	image = image.convertToFormat(QImage::Format_RGB32);
	encoder.encodeImage(image);
#endif USE_FFMPEG

//	std::cout << "nodesToBeCaptured: " << nodeViewsToBeCaptured.size() << std::endl;

	for(int i=0; i<nodeViewsToBeCaptured.size(); i++){
		QImage nodeViewImage = nodeViewsToBeCaptured[i]->getImage()->copy(0,0,256,256);
		QString name = captureDirectory + "/node%1/frame%2.jpg";
		name = name.arg(util::toQString(i+1)).arg(util::toQString(slider->value()), 4, '0');
//		 name = captureDirectory + "/node" + util::toQString(i+1) + "/frame"+ QString::arg(util::toQString(slider->value()) + ".jpg";
		nodeViewImage.save(name);

#ifdef USE_FFMPEG
//		image = image.convertToFormat(QImage::Format_RGB32);
		nodeViewEncoders[i]->encodeImage(nodeViewImage);
//		encoder.encodeImage(nodeViewImage);

#endif USE_FFMPEG
	}


}

void Window::stopCapture(){
	QCoreApplication::processEvents();

	for(int i=0; i<24; i++){
		captureFrame();
	}

	this->setMaximumSize(16777215, 16777215);
	this->setMinimumSize(0, 0);
	capture=false;
	nodeViewsToBeCaptured.clear();

#ifdef USE_FFMPEG
	foreach(QVideoEncoder* nodeViewEncoder, nodeViewEncoders){
		delete(nodeViewEncoder);
	}
	nodeViewEncoders.clear();
	encoder.close();
#endif USE_FFMPEG
}

void Window::stopScan(){
	killTimer(timerId);
	timerId=0;
	if(scannedEdge){
		scannedEdge->flash(false);
		scannedEdge->update();
	}

	if(capture) stopCapture();
}

void Window::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event);

//	while (slider->value() < 100){
	int newSliderValue = slider->value();
	if(capture){
		newSliderValue++;
	}else {
		newSliderValue = std::min(newSliderValue+3, 100);
	}
	slider->setValue(newSliderValue);
	if(capture) captureFrame();
	if(slider->value() >= 100) stopScan();
	if(scannedEdge && slider->value()%5 == 0) scannedEdge->flash(true);
//	}
}


void Window::nodeViewSelected(bool selected){
	deleteViewNodeAction->setEnabled(selected);
}

void Window::nodeSelected(bool selected){
	if(nodeIsSelected == selected) return;
	nodeIsSelected = selected;
	addLabelAction->setEnabled(selected);
	addViewNodeAction->setEnabled(selected);
	unlabelAction->setEnabled(selected);
	unlabelButton->setEnabled(selected);

}

void Window::edgeSelected(bool selected, Edge* edge){
	if(edgeIsSelected == selected) return;
	edgeIsSelected = selected;
	selectedEdge = edge;
	resetAction->setEnabled(selected);
	scanAction->setEnabled(selected);
	addLabelAction->setEnabled(selected);
	unlabelAction->setEnabled(selected);
	unlabelButton->setEnabled(selected);
	horizontalGroupBox->setEnabled(selected);
	screenCaptureAction->setEnabled(selected);
	if(selected) setSlider();
}

void Window::updateSidebarSelection(){
	bool nodeIsViewSelected = false;

	foreach(QGraphicsItem* item, sidebar->scene()->selectedItems()){
		if(qgraphicsitem_cast<NodeView*>(item)) nodeIsViewSelected = true;
	}

	nodeViewSelected(nodeIsViewSelected);
}

void Window::updateMainSelection(){
	stopScan();
	nodeSelected(false);
	edgeSelected(false, NULL);

	foreach(QGraphicsItem* item, graphWidget->scene()->selectedItems()){
		if(qgraphicsitem_cast<Node*>(item)) nodeSelected(true);
		if(qgraphicsitem_cast<Edge*>(item)) edgeSelected(true, qgraphicsitem_cast<Edge*>(item));
		if(edgeIsSelected && nodeIsSelected) break;
	}
}

void Window::unlabel(){
	undoStack.push(new CommandLabelObject(graphWidget->scene()->selectedItems(), new LabelWidget(this)));
	setWindowModified(true);
}

void Window::colorNode(QWidget* object){
	LabelWidget* colorButton = qobject_cast<LabelWidget*>(object);
	if(colorButton){
		undoStack.push(new CommandLabelObject(graphWidget->scene()->selectedItems(), colorButton));
		setWindowModified(true);
	}
}


void Window::colorNode(LabelWidget* label){
	undoStack.push(new CommandLabelObject(graphWidget->scene()->selectedItems(), label));
}

void Window::resetWeight(){
	stopScan();
	undoStack.push(new CommandSetWeight(graphWidget->scene()->selectedItems()));
	if(selectedEdge) sliderValueChangedF(selectedEdge->getWeight());
}

void Window::resetAllWeights(){
	stopScan();
	undoStack.push(new CommandSetWeight(graphWidget->scene()->items()));
	setSlider();
}

void Window::resetWeights(QList<QGraphicsItem*> items, bool batch){
	stopScan();
	undoStack.push(new CommandSetWeight(items));
	setSlider();
}

void Window::setValue(int value){
	double newWeight = (float(value))*0.06-3;
	emit sliderValueChangedF(newWeight);
}

void Window::setValueF(double weight){
	int value = round(((weight+3)/6)*100);
	emit sliderValueChanged(value);
	if(selectedEdge){
		if(selectedEdge->getWeight() != weight)
			undoStack.push(new CommandSetWeight(selectedEdge, weight));
	}

}

void Window::setLabelOny(){
	labelMode = onlyLabels;
	graphWidget->update();
}
void Window::setSignOny(){
	labelMode = onlyConnectionSign;
	graphWidget->update();
}
void Window::setBoth(){
	labelMode = both;
	graphWidget->update();
}

void Window::setSlider(){
	if(selectedEdge){
		int value = round(((selectedEdge->getWeight()+3)/6)*100);
		slider->blockSignals(true);
		slider->setValue(value);
		slider->blockSignals(false);
		spinBox->blockSignals(true);
		spinBox->setValue(selectedEdge->getWeight());
		spinBox->blockSignals(false);
	}
}

