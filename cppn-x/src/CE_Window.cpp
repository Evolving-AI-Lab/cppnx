/*
 * window.cpp
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */

#include "CE_Window.h"
#include "CE_Util.h"

const QString Window::modularityText = "Modularity (Q-score): ";
const QString Window::hierachyText = "Hierarchy: ";

Window::Window()
{

	//Create widgets
	cppnWidget = new CppnWidget(this);
	labelWidget = new LabelWidget();
    nodeviewWidget = new NodeViewWidget();
    weightWidget = new WeightWidget();

	//Connect cppn widget
//    connect(cppnWidget, SIGNAL(sceneModified()), this, SLOT(onSceneModified()));
    connect(cppnWidget, SIGNAL(requestCommandExecution(ComBase*)), this, SLOT(executeCommand(ComBase*)));
    connect(cppnWidget, SIGNAL(edgeUpdated(Edge*)), this, SLOT(updateEdge(Edge*)));
    connect(cppnWidget, SIGNAL(nodeUpdated(Node*)), this, SLOT(updateNode(Node*)));
    connect(cppnWidget, SIGNAL(newModularity(double)), this, SLOT(updateModularity(double)));
    connect(cppnWidget, SIGNAL(labelableObjectSelected(bool)), labelWidget, SLOT(labelableObjectSelected(bool)));
    connect(cppnWidget, SIGNAL(requestAddNodeview(QList<QGraphicsItem*>)), nodeviewWidget, SLOT(addNodeView(QList<QGraphicsItem*>)));
    connect(cppnWidget->scene(), SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));


	//Connect label widget
    connect(labelWidget, SIGNAL(requestCommandExecution(ComBase*)), this, SLOT(executeCommand(ComBase*)));
    connect(labelWidget, SIGNAL(applyLabel(Label*)), cppnWidget, SLOT(applyLabel(Label*)));
    connect(labelWidget, SIGNAL(labelsChanged()), cppnWidget, SLOT(updateAll()));
    connect(labelWidget, SIGNAL(selectionChanged()), cppnWidget, SLOT(deselectItems()));
    connect(labelWidget, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
//    connect(labelWidget, SIGNAL(sceneModified()), this, SLOT(onSceneModified()));


    //Connect nodeview widget
    connect(nodeviewWidget, SIGNAL(requestCommandExecution(ComBase*)), this, SLOT(executeCommand(ComBase*)));
    connect(nodeviewWidget->scene(), SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
//    connect(nodeviewWidget, SIGNAL(sceneModified()), this, SLOT(onSceneModified()));

    //Connect weight widget
    connect(weightWidget->getResetAllAction(), SIGNAL(triggered()), cppnWidget, SLOT(resetAllWeights()));
    connect(weightWidget->getFirstWeightSliderWidget(), SIGNAL(weightChanged(double)), cppnWidget, SLOT(setWeight(double)));
    connect(weightWidget, SIGNAL(flash(bool)), cppnWidget, SLOT(flash(bool)));
    connect(weightWidget, SIGNAL(scanStarted()), this, SLOT(startScan()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(scanStopped()), this, SLOT(stopScan()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(requestFilmStart()), this, SLOT(startFilm()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(filmStopped()), this, SLOT(stopFilm()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(frameReady()), this, SLOT(captureFrame()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(bookendStartChanged(double)), cppnWidget, SLOT(onBookendStartChanged(double)), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(bookendEndChanged(double)), cppnWidget, SLOT(onBookendEndChanged(double)), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(bookendStepChanged(double)), cppnWidget, SLOT(onBookendStepChanged(double)), Qt::DirectConnection);





    //Connect undostack
    connect(&undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(onCleanState(bool)));


    //Create actions
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

    undoAction = undoStack.createUndoAction(this, tr("Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    redoAction = undoStack.createRedoAction(this, tr("Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    selectAllAction = new QAction(tr("&Select all"), this);
    selectAllAction->setShortcuts(QKeySequence::SelectAll);
    selectAllAction->setStatusTip(tr("Selects all objects in active window"));
    selectAllAction->setEnabled(false);

    removeAction = new QAction(tr("&Delete"), this);
    removeAction->setShortcut(Qt::Key_Backspace);
    removeAction->setStatusTip(tr("Deletes all selected objects"));
    removeAction->setEnabled(false);

    exportImageAction = new QAction(tr("&Export nodes"), this);
    exportImageAction->setShortcut(tr("Alt+X"));
    exportImageAction->setStatusTip(tr("Exports the selected nodes as image files"));
    exportImageAction->setEnabled(false);

    util::insertFront(cppnWidget->getNodeMenu(), exportImageAction);
    util::insertFront(nodeviewWidget->getNodeviewMenu(), exportImageAction);

    util::insertFront(cppnWidget->getNodeMenu(), removeAction);
    util::insertFront(cppnWidget->getEdgeMenu(), removeAction);
    util::insertFront(labelWidget->getLabelContextMenu(), removeAction);
    util::insertFront(nodeviewWidget->getNodeviewMenu(), removeAction);

    //Create menus
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(exitAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    posMenu = new QMenu(tr("&Position"), this);
    posMenu->addAction(cppnWidget->getCircleAction());
    posMenu->addAction(cppnWidget->getONPAction());
    posMenu->addAction(cppnWidget->getLayerAction());

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();

    editMenu->addAction(removeAction);
    editMenu->addAction(selectAllAction);
    editMenu->addSeparator();

    //Edge actions:
    editMenu->addAction(weightWidget->getFirstWeightSliderWidget()->getResetAction());
    editMenu->addAction(weightWidget->getResetAllAction());
    editMenu->addSeparator();

    editMenu->addAction(weightWidget->getScanAction());
    editMenu->addAction(weightWidget->getScreenCaptureAction());
    editMenu->addSeparator();

    //Node actions

    editMenu->addAction(exportImageAction);
    editMenu->addAction(cppnWidget->getAddNodeviewAction());

    editMenu->addSeparator();
    editMenu->addMenu(labelWidget->getLabelMenu());

    viewMenu= new QMenu(tr("&View"), this);
//    viewMenu->addAction(cppnWidget->getViewGroup());
    viewMenu->addAction(cppnWidget->getLabelViewAction());
    viewMenu->addAction(cppnWidget->getSignViewAction());
    viewMenu->addAction(cppnWidget->getLabelAndSignViewAction());
    viewMenu->addSeparator();
    viewMenu->addAction(cppnWidget->getNodeLabelAction());
    viewMenu->addAction(cppnWidget->getNodeModuleAction());
    viewMenu->addSeparator();
    viewMenu->addAction(cppnWidget->getCurvedLineAction());
    viewMenu->addAction(cppnWidget->getStraightLineAction());

    menuBar = new QMenuBar;
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(viewMenu);
    menuBar->addMenu(posMenu);

    cppnWidget->getNodeMenu()->addSeparator();
    cppnWidget->getNodeMenu()->addMenu(labelWidget->getLabelMenu());

    cppnWidget->getEdgeMenu()->addAction(weightWidget->getFirstWeightSliderWidget()->getResetAction());
    cppnWidget->getEdgeMenu()->addAction(weightWidget->getScanAction());
    cppnWidget->getEdgeMenu()->addAction(weightWidget->getScreenCaptureAction());
    cppnWidget->getEdgeMenu()->addSeparator();
    cppnWidget->getEdgeMenu()->addMenu(labelWidget->getNodeContextMenu());



    //Connect actions
    connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close ()));
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(remove()));
    connect(exportImageAction, SIGNAL(triggered()), this, SLOT(exportToJpg()));


    connect(nodeviewWidget, SIGNAL(focusChanged()), this, SLOT(onSelectionChanged()));
    connect(cppnWidget, SIGNAL(focusChanged()), this, SLOT(onSelectionChanged()));

    modularityLabel = new QLabel(modularityText);
    hierachyLabel = new QLabel(hierachyText);

    leftLayout = new QVBoxLayout;
    leftLayout->addWidget(cppnWidget);
    leftLayout->addWidget(weightWidget);



    mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(nodeviewWidget, 0);
    mainLayout->addWidget(labelWidget, 0);
    setMenuBar(menuBar);

    QWidget* temp = new QWidget();
    temp->setLayout(mainLayout);
    setCentralWidget(temp);

    statusBar()->addWidget(modularityLabel);

    setWindowTitle(tr("Untitled"));



    fileLoaded(false);
}

Window::~Window(){

}


/*********************************************
 ******************* SLOTS *******************
 *********************************************/

//Loading and saving
void Window::load(){
	if(not(askSaveChanges())) return;

	QString newFileName = QFileDialog::getOpenFileName(this,
	         tr("Open Genome File"), "",
	         tr("Genome file (*.xml;*.zip);;All Files (*)"));
	if(newFileName.isEmpty()) return;

	try{
		///////////////////////////////
		CppnParser parser(newFileName.toStdString());
		parser.parse();

		std::cout << "Loading cppn" << std::endl;
		cppnWidget->setCppn(parser.getNodes(), parser.getEdges());

		std::cout << "Loading labels" << std::endl;
		labelWidget->setLabels(parser.getLabels());

		std::cout << "Loading nodeviews" << std::endl;
		nodeviewWidget->setNodeviews(parser.getNodeviews());
//		nodeviewWidget->addNodeView(cppnWidget->getCppn()->getFinalNodeView());

		std::cout << "Loading file-information" << std::endl;
		fileInformation = parser.getFileInformation();

		std::cout << "Positioning nodes" << std::endl;
		if(fileInformation->newFile) cppnWidget->positionNodesLayers();
		///////////////////////////////
		undoStack.clear();
		fileInformation->fileName = newFileName;
		fileLoaded(true);



		setWindowTitle(newFileName.split('/').back() +"[*]");
		setWindowModified(false);

		std::cout << "Load done" << std::endl;
	} catch(std::exception& e){
		QString message(("Error reading file: " + newFileName.toStdString() + "\n" + std::string( e.what() )).c_str());
		QMessageBox::information(this, tr("Unable to open file"), message);
	}
}

void Window::save(){
	if(fileInformation->fileName.isEmpty() || fileInformation->newFile){
		saveAs();
	} else {
		actualSave(fileInformation->fileName);
	}
}

void Window::saveAs(){
	QString newFileName = QFileDialog::getSaveFileName(this,
	         tr("Save Genome File"), "",
	         tr("XML File (*.xml);;All Files (*)"));
	actualSave(newFileName);
}

//Scan an screen capture
void Window::startFilm(){
	QString newFileName = util::getSupportedFilename(this, tr("Chose save directory"));
	if(newFileName.isEmpty()) return;
	captureDirectory = util::getBase(newFileName);
	extention = util::getExtention(newFileName);
	QDir().mkdir(captureDirectory);

	nodeViewsToBeCaptured.clear();

	if(this->isMaximized()) this->move(0,0);
	this->setFixedSize(this->width()+this->width()%2,this->height()+this->height()%2);

	QDir().mkdir(captureDirectory + "/fullApplication");

#ifdef USE_FFMPEG
	nodeViewEncoders.clear();
	encoder.createFile(captureDirectory + "/fullApplication.mpg",this->width(),this->height(),this->width()*this->height()*4,10);
#endif USE_FFMPEG

	//Select the final nodeview
	QList<QGraphicsItem*> allNodeviews = nodeviewWidget->scene()->items();
	foreach(QGraphicsItem* item, allNodeviews){
//		std::cout << "Searching for finalnodeview..." << std::endl;
		FinalNodeView* finalnodeView = qgraphicsitem_cast<FinalNodeView*>(item);
		if(finalnodeView){
//			std::cout << "Final nodeview found!" << std::endl;
			finalnodeView->setSelected(true);
		}
	}


	QList<QGraphicsItem*> graphicsItemsToBeEncoded = nodeviewWidget->scene()->selectedItems();
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

	frame=0;
	weightWidget->startFilm();
}

void Window::stopFilm(){
	QCoreApplication::processEvents();

	for(int i=0; i<24; i++){
		captureFrame();
	}

	this->setMaximumSize(16777215, 16777215);
	this->setMinimumSize(0, 0);
//	capture=false;
	nodeViewsToBeCaptured.clear();

#ifdef USE_FFMPEG
	foreach(QVideoEncoder* nodeViewEncoder, nodeViewEncoders){
		delete(nodeViewEncoder);
	}
	nodeViewEncoders.clear();
	encoder.close();
#endif USE_FFMPEG
}

void Window::captureFrame(){
	//		QPixmap test = QPixmap::grabWindow(this->winId(), 0, 0, 1024, 768);
	QPixmap test = QPixmap::grabWidget(this, 0, 0, this->width(), this->height());
	QString name = captureDirectory + "/fullApplication/frame%1" + extention;
	name = name.arg(util::toQString(frame), 4, '0');
//	std::cout << name.toStdString() << std::endl;
	test.save(name);

#ifdef USE_FFMPEG
	QImage image = test.toImage();
	image = image.convertToFormat(QImage::Format_RGB32);
	encoder.encodeImage(image);
#endif USE_FFMPEG

//	std::cout << "nodesToBeCaptured: " << nodeViewsToBeCaptured.size() << std::endl;

	for(int i=0; i<nodeViewsToBeCaptured.size(); i++){
		QImage nodeViewImage = nodeViewsToBeCaptured[i]->getImage()->copy(0,0,256,256);
		QString name = captureDirectory + "/node%1/frame%2" + extention;
		name = name.arg(util::toQString(i+1)).arg(util::toQString(frame), 4, '0');
//		std::cout << name.toStdString() << std::endl;
		nodeViewImage.save(name);

#ifdef USE_FFMPEG
//		image = image.convertToFormat(QImage::Format_RGB32);
		nodeViewEncoders[i]->encodeImage(nodeViewImage);
//		encoder.encodeImage(nodeViewImage);

#endif USE_FFMPEG
	}
	frame++;
}


void Window::startScan(){
	cppnWidget->setInteractive(false);
	nodeviewWidget->setInteractive(false);
	labelWidget->setEnabled(false);
}

void Window::stopScan(){
	cppnWidget->setInteractive(true);
	nodeviewWidget->setInteractive(true);
	labelWidget->setEnabled(true);
}

//Update functions
void Window::updateEdge(Edge* edge){
//	std::cout << edge << std::endl;
	if(edge){
		weightWidget->edgeSelected(true);
		weightWidget->setEdge(edge->getWeight(),
				edge->getOriginalWeight(),
				util::toQString(edge->getId() + "_" + edge->getBranch()),
				edge->getBookendStart(),
				edge->getBookendEnd(),
				edge->getStepsize());
	} else {
		weightWidget->edgeSelected(false);
	}
}

void Window::updateNode(Node* node){
//	std::cout << edge << std::endl;
	if(node){
		weightWidget->edgeSelected(false);
		weightWidget->setNode(util::toQString(node->getBranch()  + "_" + node->getId()));
	} else {

	}
}

void Window::executeCommand(ComBase* command){
//	std::cout << "Command pushed: " << command->text().toStdString() << std::endl;
	if(command->isOk()){
		undoStack.push(command);
	} else {
		delete command;
	}
}

/*********************************************
 ************* PRIVATE FUNCTIONS *************
 *********************************************/

void Window::actualSave(const QString& fileName){
	if(fileName.isEmpty()) return;
	try{
		CppnWriter writer(fileName.toStdString());
		writer.write(cppnWidget->getCppn(), labelWidget->getLabels(), fileInformation, nodeviewWidget->getNodeviews());
		fileInformation->newFile = false;
		fileInformation->fileName = fileName;
		setWindowModified(false);
		setWindowTitle(fileInformation->fileName.split('/').back() + "[*]");
		undoStack.setClean();

	} catch(std::exception& e){
		QString message(("Error saving file: " + fileName.toStdString() + "\n" + std::string( e.what() )).c_str());
		QMessageBox::information(this, tr("Unable to open file"), message);
	}
}

bool Window::askSaveChanges(){
	if(isWindowModified ()){
		 QMessageBox msgBox(QMessageBox::Question,tr("Save as"),"The file " + fileInformation->fileName +  " has been modified.", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this, Qt::Dialog);
		 msgBox.setWindowModality ( Qt::WindowModal);

		 msgBox.setText("The file " + fileInformation->fileName +  " has been modified.");
		 msgBox.setInformativeText("Do you want to save your changes?");
		 msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		 msgBox.setDefaultButton(QMessageBox::Save);
		 int ret = msgBox.exec();

		 switch (ret) {
		   case QMessageBox::Save:
		       save();
		       break;
		   case QMessageBox::Discard:
		       break;
		   case QMessageBox::Cancel:
			   return false;
		       break;
		   default:
		       // should never be reached
		       break;
		 }
	}

	return true;
}

void Window::closeEvent(QCloseEvent * event){
	event->setAccepted(askSaveChanges());
}


void Window::fileLoaded(bool selected){
	saveAction->setEnabled(selected);
	saveAsAction->setEnabled(selected);

	labelWidget->setEnabled(selected);
	cppnWidget->setEnabled(selected);
	weightWidget->setEnabled(selected);
	nodeviewWidget->setEnabled(selected);
}

void Window::onSceneModified(){
//	setWindowModified(true);
}

void Window::onCleanState(bool state){
	setWindowModified(!state);
}

void Window::selectAll(){
	QWidget* widget =  QApplication::focusWidget ();
	if(widget){
		QGraphicsView* graphicsView = qobject_cast<QGraphicsView*> (widget);
		if(graphicsView){
			foreach(QGraphicsItem* item, graphicsView->scene()->items()){
				item->setSelected(true);
			}
		}
	}
}

void Window::remove(){
	QWidget* widget =  QApplication::focusWidget ();
	if(widget){
		NodeViewWidget* nodeviewWidget = qobject_cast<NodeViewWidget*> (widget);
		if(nodeviewWidget){
			nodeviewWidget->deleteNodeView();
			return;
		}
		CppnWidget* cppnWidget = qobject_cast<CppnWidget*> (widget);
		if(cppnWidget){
			cppnWidget->deleteObjects();
			return;
		}
		DragAndDropGraphicsView* dargAndDropGraphicsView = qobject_cast<DragAndDropGraphicsView*> (widget);
		if(dargAndDropGraphicsView){
			labelWidget->requestDelete();
			return;
		}
	}
}

void Window::exportToJpg(){
	if(nodeviewWidget->getIsActive()){
		nodeviewWidget->saveImage();
	} else if(cppnWidget->getIsActive()){
		cppnWidget->saveImage();
	}
}

void Window::onSelectionChanged(){
	bool nodeViewSelected = (nodeviewWidget->getIsActive() && nodeviewWidget->scene()->selectedItems().count() > 0);
	bool deletableNodeviewSelected = (nodeviewWidget->getIsActive() && nodeviewWidget->deletableNodeviewSelected());

	bool nodeSelected = (cppnWidget->getIsActive() && cppnWidget->getNodeSelected());
	bool edgeSelected = (cppnWidget->getIsActive() && cppnWidget->getEdgeSelected());
	bool labelSelected = (labelWidget->getGraphicsViewHasFocus() && labelWidget->getLabelSelected());

	exportImageAction->setEnabled(nodeViewSelected || nodeSelected);
	removeAction->setEnabled(deletableNodeviewSelected || nodeSelected || edgeSelected || labelSelected);
	selectAllAction->setEnabled(nodeviewWidget->hasFocus() || cppnWidget->hasFocus() || labelWidget->getGraphicsViewHasFocus());
}

void Window::updateModularity(double qscore){
	modularityLabel->setText(modularityText + util::toQString(qscore));
}
