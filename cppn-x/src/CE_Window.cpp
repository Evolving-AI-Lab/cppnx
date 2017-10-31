/*
 * window.cpp
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */

//QT includes
#include <QSettings>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

//Local includes
#include "CE_Window.h"
#include "CE_Util.h"
#include "CX_ModuleColor.h"
#include "CX_Random.hpp"
#include "CX_Hierarchy.hpp"
#include "CX_Debug.hpp"
#include "CX_PreferencesWidget.hpp"
#include "CX_CsvParser.hpp"
#include "CX_Analysis.hpp"
#include "CX_Timer.hpp"
#include "CX_GenomeDirWidget.hpp"
#include "CX_DownloadAssistant.hpp"

//Standard includes
#include <cmath>
#include <time.h>

// Debug output Macro's so debug statements aren't as long
#define DBOA dbg::out(dbg::info, "analysis")
#define DBOW dbg::out(dbg::info, "window")

const QString Window::modularityText = "Modularity (Q-score): ";
const QString Window::hierachyText = "Hierarchy: ";
const QString Window::generationText = "Generation: ";
const QString Window::branchText = "Branch: ";
const QString Window::url = "http://www.cs.uwyo.edu/~jeffclune"
		"/evolvingai/picbreeder_genomes_2014/genomeAll";

using namespace glb_settings;

Window::Window(){
    dbg::trace trace("window", DBG_HERE);

    //Load settings
    _settingsFile = QApplication::applicationDirPath() + "/demosettings.ini";
    loadSettings();

	//Create widgets
	cppnWidget = new CppnWidget(this);
	labelWidget = new LabelWidget();
    nodeviewWidget = new NodeViewWidget();
    weightWidget = new WeightWidget();

    /***********************************
     ******** Connect widgets **********
     ***********************************/

	//Connect cppn widget
    connect(cppnWidget, SIGNAL(requestCommandExecution(ComBase*)),
    		this, SLOT(executeCommand(ComBase*)));
    connect(cppnWidget, SIGNAL(edgeUpdated(Edge*)),
    		weightWidget, SLOT(updateEdge(Edge*)));
    connect(cppnWidget, SIGNAL(nodeUpdated(Node*)),
    		weightWidget, SLOT(updateNode(Node*)));
    connect(cppnWidget, SIGNAL(newModularity(double)),
    		this, SLOT(updateModularity(double)));
    connect(cppnWidget, SIGNAL(labelableObjectSelected(bool)),
    		labelWidget, SLOT(labelableObjectSelected(bool)));
    connect(cppnWidget, SIGNAL(requestAddNodeview(QList<QGraphicsItem*>)),
    		nodeviewWidget, SLOT(addNodeView(QList<QGraphicsItem*>)));
    connect(cppnWidget->scene(), SIGNAL(selectionChanged()),
    		this, SLOT(onSelectionChanged()));
    connect(cppnWidget, SIGNAL(focusChanged()),
    		this, SLOT(onSelectionChanged()));

	//Connect label widget
    connect(labelWidget, SIGNAL(requestCommandExecution(ComBase*)),
    		this, SLOT(executeCommand(ComBase*)));
    connect(labelWidget, SIGNAL(applyLabel(Label*)),
    		cppnWidget, SLOT(applyLabel(Label*)));
    connect(labelWidget, SIGNAL(labelsChanged()),
    		cppnWidget, SLOT(updateAll()));
    connect(labelWidget, SIGNAL(selectionChanged()),
    		cppnWidget, SLOT(deselectItems()));
    connect(labelWidget, SIGNAL(selectionChanged()),
    		this, SLOT(onSelectionChanged()));


    //Connect nodeview widget
    connect(nodeviewWidget, SIGNAL(requestCommandExecution(ComBase*)),
    		this, SLOT(executeCommand(ComBase*)));
    connect(nodeviewWidget->scene(), SIGNAL(selectionChanged()),
    		this, SLOT(onSelectionChanged()));
    connect(nodeviewWidget, SIGNAL(focusChanged()),
    		this, SLOT(onSelectionChanged()));


    //Connect weight widget
    connect(weightWidget->getResetAllAction(), SIGNAL(triggered()),
    		cppnWidget, SLOT(resetAllWeights()));
    connect(weightWidget->getFirstWeightSliderWidget(),
    		SIGNAL(weightChanged(double)), cppnWidget, SLOT(setWeight(double)));
    connect(weightWidget, SIGNAL(flash(bool)),
    		cppnWidget, SLOT(flash(bool)));
    connect(weightWidget, SIGNAL(scanStarted()),
    		this, SLOT(startScan()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(scanStopped()),
    		this, SLOT(stopScan()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(requestFilmStart()),
    		this, SLOT(startFilm()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(filmStopped()),
    		this, SLOT(stopFilm()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(frameReady()),
    		this, SLOT(captureFrame()), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(bookendStartChanged(double)), cppnWidget,
    		SLOT(onBookendStartChanged(double)), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(bookendEndChanged(double)), cppnWidget,
    		SLOT(onBookendEndChanged(double)), Qt::DirectConnection);
    connect(weightWidget, SIGNAL(bookendStepChanged(double)), cppnWidget,
    		SLOT(onBookendStepChanged(double)), Qt::DirectConnection);

    //Connect undostack
    connect(&undoStack, SIGNAL(cleanChanged(bool)),
    		this, SLOT(onCleanState(bool)));


    /***********************************
     ********* Create Actions **********
     ***********************************/
    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close ()));

    _newAction = new QAction(tr("&New..."), this);
    _newAction->setShortcuts(QKeySequence::New);
    _newAction->setStatusTip(tr("Create a new CPPN"));
    connect(_newAction, SIGNAL(triggered()), this, SLOT(createNew()));

    loadAction = new QAction(tr("&Load..."), this);
    loadAction->setShortcuts(QKeySequence::Open);
    loadAction->setStatusTip(tr("Load an existing file"));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));

    loadIDAction = new QAction(tr("&Load ID..."), this);
    loadIDAction->setShortcut(tr("Ctrl+Alt+O"));
    loadIDAction->setStatusTip(tr("Load a genome by ID"));
    connect(loadIDAction, SIGNAL(triggered()), this, SLOT(loadByID()));

    _preferencesAction= new QAction(tr("&Preferences..."), this);
    _preferencesAction->setMenuRole(QAction::PreferencesRole);
    connect(_preferencesAction, SIGNAL(triggered()), this, SLOT(preferences()));

    _nextGeneration = new QAction(tr("Next generation"), this);
    _nextGeneration->setShortcut(tr("Ctrl+>"));
    _nextGeneration->setStatusTip(tr("If the current genome was loaded by ID, "
    		"go to the next generation."));
    connect(_nextGeneration, SIGNAL(triggered()), this, SLOT(nextGeneration()));

    _previousGeneration = new QAction(tr("Previous generation"), this);
    _previousGeneration->setShortcut(tr("Ctrl+<"));
    _previousGeneration->setStatusTip(tr("If the current genome was loaded by "
    		"ID, go to the previous generation."));
    connect(_previousGeneration, SIGNAL(triggered()),
    		this, SLOT(previousGeneration()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the genome to disk"));
    saveAction->setDisabled(true);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(tr("&Save as..."), this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save the genome to disk"));
    saveAsAction->setDisabled(true);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    undoAction = undoStack.createUndoAction(this, tr("Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    redoAction = undoStack.createRedoAction(this, tr("Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    selectAllAction = new QAction(tr("&Select all"), this);
    selectAllAction->setShortcuts(QKeySequence::SelectAll);
    selectAllAction->setStatusTip(tr("Selects all objects in active window"));
    selectAllAction->setEnabled(false);
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

    removeAction = new QAction(tr("&Delete"), this);
    removeAction->setShortcut(Qt::Key_Backspace);
    removeAction->setStatusTip(tr("Deletes all selected objects"));
    removeAction->setEnabled(false);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(remove()));

    exportImageAction = new QAction(tr("&Export nodes"), this);
    exportImageAction->setShortcut(tr("Alt+X"));
    exportImageAction->setStatusTip(tr("Exports the selected nodes as image "
    		"files"));
    exportImageAction->setEnabled(false);
    connect(exportImageAction, SIGNAL(triggered()), this, SLOT(exportToJpg()));

    _startAnalysisAction = new QAction(tr("&Start analysis"), this);
    _startAnalysisAction->setStatusTip(tr("Starts the analysis of a folder. "
    		"WARNING: Long and experimental procedure!"));
    _startAnalysisAction->setEnabled(true);
    connect(_startAnalysisAction, SIGNAL(triggered()),
    		this, SLOT(startAnalysis()));

    _nullModelAction = new QAction(tr("&Create null-model"), this);
    _nullModelAction->setStatusTip(tr("Starts the analysis of a folder. "
    		"WARNING: Long and experimental procedure!"));
    _nullModelAction->setEnabled(true);
    connect(_nullModelAction, SIGNAL(triggered()),
    		this, SLOT(createNullModel()));

    _compareGenomesAction = new QAction(tr("&Mark differences..."), this);
    _compareGenomesAction->setStatusTip(tr("Marks the nodes that are not in "
    		"the chosen genome."));
    _compareGenomesAction->setEnabled(true);
    connect(_compareGenomesAction, SIGNAL(triggered()),
    		this, SLOT(markDifferences()));

    _importPositionsAction = new QAction(tr("Import positions..."), this);
    _importPositionsAction->setStatusTip(tr("Import the positions from "
    		"another genome file."));
    _importPositionsAction->setEnabled(true);
    connect(_importPositionsAction, SIGNAL(triggered()),
    		this, SLOT(importPositions()));

    _importLabelsAction = new QAction(tr("Import labels..."), this);
    _importLabelsAction->setStatusTip(tr("Import the labels from another "
    		"genome file."));
    _importLabelsAction->setEnabled(true);
    connect(_importLabelsAction, SIGNAL(triggered()),
    		this, SLOT(importLabels()));

    _createModuleLabelsAction = new QAction(tr("Create module labels"), this);
    _createModuleLabelsAction->setStatusTip(tr("Create labels based on the "
    		"modules in the genome. Overwrites current labels."));
    _createModuleLabelsAction->setEnabled(true);
    connect(_createModuleLabelsAction, SIGNAL(triggered()),
    		this, SLOT(createModuleLabels()));

    _toggleLegendAction = _createAction(tr("Toggle legend"),
    		tr("Toggles whether a legend is shown."), SLOT(toggleLegend()));
    _toggleLegendAction->setShortcut(tr("Shift+4"));
    _toggleLegendAction->setCheckable(true);
    cppnWidget->addAction(_toggleLegendAction);

    _exportToImageSeriesAction = _createAction(tr("Export to image series"),
            tr("Scan over the selected connection and "
            		"export the resulting images"),
            SLOT(exportToImageSeries()));

    _filmEveryConnectionAction = _createAction(tr("Film every connection"),
            tr("Create a film for every connection"),
            SLOT(filmEveryConnection()));


    /***********************************
     ** Add actions to other widgets ***
     ***********************************/
    util::insertFront(cppnWidget->getNodeMenu(), exportImageAction);
    util::insertFront(nodeviewWidget->getNodeviewMenu(), exportImageAction);

    util::insertFront(cppnWidget->getNodeMenu(), removeAction);
    util::insertFront(cppnWidget->getEdgeMenu(), removeAction);
    util::insertFront(labelWidget->getLabelContextMenu(), removeAction);
    util::insertFront(nodeviewWidget->getNodeviewMenu(), removeAction);

    /***********************************
     ********* Create menus ************
     ***********************************/
    // File menu
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(_newAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(loadIDAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(cppnWidget->getSnapshotAction());
    fileMenu->addSeparator();
    fileMenu->addAction(_compareGenomesAction);
    fileMenu->addAction(_importPositionsAction);
    fileMenu->addAction(_importLabelsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(_nextGeneration);
    fileMenu->addAction(_previousGeneration);
    fileMenu->addSeparator();
    fileMenu->addAction(_preferencesAction);


    // Position menu
    posMenu = new QMenu(tr("&Position"), this);
    posMenu->addAction(cppnWidget->getCircleAction());
    posMenu->addAction(cppnWidget->getONPAction());
    posMenu->addAction(cppnWidget->getLayerAction());
    posMenu->addSeparator();
    posMenu->addAction(cppnWidget->getAlignHorizontalAction());
    posMenu->addAction(cppnWidget->getAlignVerticalAction());
    posMenu->addAction(cppnWidget->getSpaceHorizontalAction());
    posMenu->addAction(cppnWidget->getSpaceVerticalAction());


    // Edit menu
    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(removeAction);
    editMenu->addAction(selectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(weightWidget->getFirstWeightSliderWidget()->getResetAction());
    editMenu->addAction(weightWidget->getResetAllAction());
    editMenu->addSeparator();
    editMenu->addAction(weightWidget->getScanAction());
    editMenu->addAction(weightWidget->getScreenCaptureAction());
    editMenu->addSeparator();
    editMenu->addAction(exportImageAction);
    editMenu->addAction(cppnWidget->getAddNodeviewAction());
    editMenu->addSeparator();
    editMenu->addMenu(labelWidget->getLabelMenu());
    editMenu->addAction(_createModuleLabelsAction);
    editMenu->addSeparator();
    editMenu->addAction(cppnWidget->getAddNodeAction());
    editMenu->addAction(cppnWidget->getAddEdgeAction());
    editMenu->addMenu(cppnWidget->getAfMenu());

    // View menu
    viewMenu= new QMenu(tr("&View"), this);
    viewMenu->addAction(cppnWidget->getEdgeShowLabelAction());
    viewMenu->addAction(cppnWidget->getEdgeNoLabelAction());
    viewMenu->addAction(cppnWidget->getEdgeModuleAction());
    viewMenu->addSeparator();
    viewMenu->addAction(cppnWidget->getEdgeShowSignAction());
    viewMenu->addAction(cppnWidget->getEdgeShowSignIfNoLabelAction());
    viewMenu->addAction(cppnWidget->getEdgeNoSignAction());
    viewMenu->addSeparator();
    viewMenu->addAction(cppnWidget->getCurvedLineAction());
    viewMenu->addAction(cppnWidget->getStraightLineAction());
    viewMenu->addSeparator();
    viewMenu->addAction(cppnWidget->getWeightEdgeWidthAction());
    viewMenu->addAction(cppnWidget->getFixedEdgeWidthAction());
    viewMenu->addSeparator();
    viewMenu->addAction(cppnWidget->getNodeLabelAction());
    viewMenu->addAction(cppnWidget->getNodeModuleAction());
    viewMenu->addAction(cppnWidget->getNodeLabelNoImageAction());
    viewMenu->addSeparator();
    viewMenu->addAction(_toggleLegendAction);
    viewMenu->addAction(cppnWidget->getScaleLegendAction());
    viewMenu->addSeparator();
    viewMenu->addAction(cppnWidget->getToggleAnnotationsAction());
    viewMenu->addSeparator();
    viewMenu->addAction(cppnWidget->getZoomInAction());
    viewMenu->addAction(cppnWidget->getZoomOutAction());
    viewMenu->addAction(cppnWidget->getSetZoomAction());
    viewMenu->addSeparator();


    // Search menu
    _searchMenu = new QMenu(tr("&Search"), this);
    _searchMenu->addAction(cppnWidget->getFindUnlabeledEdgeAction());
    _searchMenu->addAction(cppnWidget->getFindEdgeByIdAction());

    // Experimental menu
    _experimentalMenu= new QMenu(tr("&Experimental"), this);

    // While being able to adjust curves, I currently have no interface for it,
    // nor is this information saved when writing to file, so disabled
//    _experimentalMenu->addAction(cppnWidget->getIncreaseCurveOffsetAction());
//    _experimentalMenu->addAction(cppnWidget->getDecreaseCurveOffsetAction());

    // I doubt any user would have much use for this particular action
    _experimentalMenu->addAction(_startAnalysisAction);
    _experimentalMenu->addAction(_nullModelAction);
    _experimentalMenu->addAction(_exportToImageSeriesAction);
#ifdef USE_FFMPEG
    _experimentalMenu->addAction(_filmEveryConnectionAction);
#endif
    _experimentalMenu->addAction(cppnWidget->getColorPathAction());

    //Menu bar
    menuBar = new QMenuBar;
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(viewMenu);
    menuBar->addMenu(posMenu);
    menuBar->addMenu(_searchMenu);
    menuBar->addMenu(_experimentalMenu);

    //cppn widget node menu
    cppnWidget->getNodeMenu()->addSeparator();
    cppnWidget->getNodeMenu()->addMenu(labelWidget->getLabelMenu());

    //cppn widget edge menu
    cppnWidget->getEdgeMenu()->addAction(weightWidget->getFirstWeightSliderWidget()->getResetAction());
    cppnWidget->getEdgeMenu()->addAction(weightWidget->getScanAction());
    cppnWidget->getEdgeMenu()->addAction(weightWidget->getScreenCaptureAction());
    cppnWidget->getEdgeMenu()->addSeparator();
    cppnWidget->getEdgeMenu()->addMenu(labelWidget->getNodeContextMenu());


    /***********************************
     ************* Misc ****************
     ***********************************/

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

    _currentBranch = -1;
    _currentGeneration = -1;
    _maxGeneration = -1;
    _parent = -1;
    _qscore = 0.0;
}

Window::~Window(){
    dbg::trace trace("window", DBG_HERE);
}


/*********************************************
 ******************* SLOTS *******************
 *********************************************/

void Window::createNew(){
    dbg::out(dbg::info, "window") << "Loading cppn" << std::endl;
    QList<Node*> nodes;
    id_t id = 0;
    Node* in_x = new Node("0", util::toString(id++), XML_TYPE_INPUT, XML_LINEAR, INPUT_X);
    Node* in_y = new Node("0", util::toString(id++), XML_TYPE_INPUT, XML_LINEAR, INPUT_Y);
    Node* in_d = new Node("0", util::toString(id++), XML_TYPE_INPUT, XML_LINEAR, INPUT_D);
    Node* in_b = new Node("0", util::toString(id++), XML_TYPE_INPUT, XML_LINEAR, INPUT_BIAS);
    Node* out_ink = new Node("0", util::toString(id++), XML_TYPE_OUTPUT, XML_LINEAR, OUTPUT_INK);
    nodes.push_back(in_x);
    nodes.push_back(in_y);
    nodes.push_back(in_d);
    nodes.push_back(in_b);
    nodes.push_back(out_ink);

    QList<Edge*> edges;
    edges.push_back(new Edge("0", util::toString(id++), in_x, out_ink));
    edges.push_back(new Edge("0", util::toString(id++), in_y, out_ink));
    edges.push_back(new Edge("0", util::toString(id++), in_d, out_ink));
    edges.push_back(new Edge("0", util::toString(id++), in_b, out_ink));
    cppnWidget->setCppn(nodes, edges);

    dbg::out(dbg::info, "window") << "Loading labels" << std::endl;
    QList<Label*> labels;
    labelWidget->setLabels(labels);

    dbg::out(dbg::info, "window") << "Loading nodeviews" << std::endl;
    QList<NodeView*> nodeviews;
    FinalNodeView* finalNodeview = new FinalNodeView();
    finalNodeview->setValueNode(out_ink);
    nodeviews.push_back(finalNodeview);
    nodeviewWidget->setNodeviews(nodeviews);

    dbg::out(dbg::info, "window") << "Loading file-information" << std::endl;
    fileInformation = new FileInformation(true);

    dbg::out(dbg::info, "window") << "Positioning nodes" << std::endl;
    cppnWidget->positionNodesLayers();

    undoStack.clear();
    fileInformation->fileName = "";
    fileLoaded(true);

    setWindowTitle("new file [*]");
    setWindowModified(false);

    dbg::out(dbg::info, "window") << "Load done" << std::endl;
}


//Loading and saving
void Window::load(){
    dbg::trace trace("window", DBG_HERE);
	if(not(askSaveChanges())) return;

	QString newFileName = QFileDialog::getOpenFileName(this,
	         tr("Open Genome File"), "",
	         tr("Genome file (*.xml;*.zip;*.csv);;All Files (*)"));
	if(newFileName.isEmpty()) return;

	loadFile(newFileName);

    _currentBranch = -1;
    _currentGeneration = -1;
    _maxGeneration = -1;
    _parent = -1;
}


void Window::loadFile(QString filename, int generation){
    dbg::trace trace("window", DBG_HERE);
	try{
	    QFileInfo fileInfo(filename);
	    if(fileInfo.suffix() == QString("csv")){
	    	CsvParser parser(filename);
	    	parser.parse(generation);

	    	DBOW << "Loading cppn" << std::endl;
	    	if(cppnWidget->getLegend()){
	    		cppnWidget->removeLegend();
	    		_toggleLegendAction->setChecked(false);
	    	}
	    	CppnInformation* info = parser.takeCppnInformation();
	    	QList<Edge*> edges = parser.takeEdges();
	    	QList<Node*> nodes = parser.takeNodes();
	    	cppnWidget->setCppn(nodes, edges, info);

	    	DBOW << "Loading labels" << std::endl;
	    	labelWidget->setLabels(parser.getLabels());

	    	DBOW << "Loading nodeviews" << std::endl;
	    	nodeviewWidget->setNodeviews(parser.takeNodeviews());

	    	DBOW << "Loading file-information" << std::endl;
	    	fileInformation = parser.takeFileInformation();
	    } else {
	    	CppnParser parser(filename.toStdString());
	    	parser.parse(generation);

	    	DBOW << "Loading cppn" << std::endl;
	    	if(cppnWidget->getLegend()){
	    		cppnWidget->removeLegend();
	    		_toggleLegendAction->setChecked(false);
	    	}
	    	cppnWidget->setCppn(parser.takeNodes(), parser.takeEdges());

	    	DBOW << "Loading labels" << std::endl;
	    	labelWidget->setLabels(parser.getLabels());

	    	DBOW << "Loading nodeviews" << std::endl;
	    	nodeviewWidget->setNodeviews(parser.takeNodeviews());

	    	DBOW << "Loading file-information" << std::endl;
	    	fileInformation = parser.takeFileInformation();
	    }

	    DBOW << "Positioning nodes" << std::endl;
		if(fileInformation->newFile) cppnWidget->positionNodesLayers();

		undoStack.clear();
		fileInformation->fileName = filename;
		fileLoaded(true);

		setWindowTitle(filename.split('/').back() +"[*]");
		setWindowModified(false);

		DBOW << "Load done" << std::endl;
	} catch(std::exception& e){
		QString message(("Error reading file: " + filename.toStdString() +
				"\n" + std::string( e.what() )).c_str());
		QMessageBox::information(this, tr("Unable to open file"), message);
	}
}


void Window::markDifferences(){
    dbg::trace trace("window", DBG_HERE);
    QString newFileName = QFileDialog::getOpenFileName(this,
             tr("Chose file to compare with"), "",
             tr("Genome file (*.xml;*.zip);;All Files (*)"));
    if(newFileName.isEmpty()) return;

    _markDifferences(newFileName);
}


void Window::importPositions(){
    dbg::trace trace("window", DBG_HERE);
    QString newFileName = QFileDialog::getOpenFileName(this,
             tr("Chose file to import positions from"), "",
             tr("Genome file (*.xml;*.zip);;All Files (*)"));
    if(newFileName.isEmpty()) return;

    _importPositions(newFileName);
}


void Window::importLabels(){
    dbg::trace trace("window", DBG_HERE);
    QString newFileName = QFileDialog::getOpenFileName(this,
             tr("Chose file to import labels from"), "",
             tr("Genome file (*.xml;*.zip);;All Files (*)"));
    if(newFileName.isEmpty()) return;

    _importLabels(newFileName);
}


void Window::loadSettings(){
    dbg::trace trace("window", DBG_HERE);
    globalSettings->load();
}


void Window::saveSettings(){
    dbg::trace trace("window", DBG_HERE);
    globalSettings->save();
}


void Window::_markDifferences(QString filename, int generation){
    dbg::trace trace("window", DBG_HERE);
    try{
        CppnParser parser(filename.toStdString());
        parser.parse(generation);

        Label* newLabel = new Label("New", QColor(200, 200, 0), false);
        Label* oldLabel = new Label("Unchanged", QColor(0, 0, 200), false);
        Label* wcLabel = new Label("Weight changed", QColor(0, 200, 0), false);

        labelWidget->clear();
        labelWidget->addLabel(newLabel);
        labelWidget->addLabel(oldLabel);
        labelWidget->addLabel(wcLabel);

        foreach(Node* nativeNode, cppnWidget->getCppn()->getNodes()){
            bool found = false;
            foreach(Node* foreignNode, parser.getNodes()){
                if(foreignNode->getId() == nativeNode->getId()){
                    nativeNode->setLabel(oldLabel);
                    found = true;
                }
            }
            if(!found){
                nativeNode->setLabel(newLabel);
            }
        }

        foreach(Edge* nativeEdge, cppnWidget->getCppn()->getEdges()){
            bool found = false;
            foreach(Edge* foreignEdge, parser.getEdges()){
                if(foreignEdge->getId() == nativeEdge->getId()){
                    if(foreignEdge->getWeight() == nativeEdge->getWeight()){
                        nativeEdge->setLabel(oldLabel);
                    } else {
                        nativeEdge->setLabel(wcLabel);
                    }

                    found = true;
                }
            }
            if(!found){
                nativeEdge->setLabel(newLabel);
            }
        }
    } catch(std::exception& e){
        QString message(("Error reading file: " + filename.toStdString() +
        		"\n" + std::string( e.what() )).c_str());
        QMessageBox::information(this, tr("Unable to open file"), message);
    }
}


void Window::_importPositions(QString filename, int generation){
    dbg::trace trace("window", DBG_HERE);
    try{
        CppnParser parser(filename.toStdString());
        parser.parse(generation);

        foreach(Node* nativeNode, cppnWidget->getCppn()->getNodes()){
            foreach(Node* foreignNode, parser.getNodes()){
                if(foreignNode->getId() == nativeNode->getId()){
                    nativeNode->setPos(foreignNode->pos());
                }
            }
        }
        executeCommand(new CommandSetPos(cppnWidget->scene()->items()));
    } catch(std::exception& e){
        QString message(("Error reading file: " + filename.toStdString() +
        		"\n" + std::string( e.what() )).c_str());
        QMessageBox::information(this, tr("Unable to open file"), message);
    }
}


void Window::_importLabels(QString filename, int generation){
    dbg::trace trace("window", DBG_HERE);
    CppnParser parser(filename.toStdString());
    try{
        parser.parse(generation);
    } catch(std::exception& e){
        QString message(("Error reading file: " + filename.toStdString() +
        		"\n" + std::string( e.what() )).c_str());
        QMessageBox::information(this, tr("Unable to open file"), message);
        return;
    }

    //Build a command so we can undo this action
    ComBase *ilAction = new ComBase();
    ilAction->setText("import labels");

    CommandAddLabel* removeLabels = new CommandAddLabel(ilAction);
    removeLabels->init(labelWidget, labelWidget->getLabels(), false);

    // To make sure the imported labels are not connected to non-existing edges
    // and nodes, we don't use the labels directly. Instead we make copies of
    // these labels, were the copies will not inherit any qt connections.
    CommandAddLabel* addLabels = new CommandAddLabel(ilAction);
    QList<Label*> importedLabels;
    QMap<Label*, Label*> labelMap;
    foreach(Label* label , parser.getLabels()){
        Label* copyLabel = new Label();
        copyLabel->copy(label);
        importedLabels.append(copyLabel);
        labelMap[label] = copyLabel;
    }
    addLabels->init(labelWidget, importedLabels, true);

    foreach(Node* nativeNode, cppnWidget->getCppn()->getNodes()){
        foreach(Node* foreignNode, parser.getNodes()){
            if(foreignNode->getId() == nativeNode->getId()){
                CommandLabelObject* co = new CommandLabelObject(ilAction);
                co->init(nativeNode, labelMap[foreignNode->getLabel()]);
            }
        }
    }

    foreach(Edge* nativeEdge, cppnWidget->getCppn()->getEdges()){
        foreach(Edge* foreignEdge, parser.getEdges()){
            if(foreignEdge->getId() == nativeEdge->getId()){
                CommandLabelObject* co = new CommandLabelObject(ilAction);
                co->init(nativeEdge, labelMap[foreignEdge->getLabel()]);
            }
        }
    }

#ifdef DBG_ENABLED
    for(int i=0; i<ilAction->childCount(); ++i){
        DBOW << ilAction->child(i)->text().toStdString() << std::endl;
    }
#endif

    executeCommand(ilAction);
}


void Window::createModuleLabels(){
    dbg::trace trace("window", DBG_HERE);
    size_t numberOfModules = cppnWidget->getNrOfModules();
    std::vector<Label*> moduleLabels;

    for(size_t i=0; i<numberOfModules; i++){
        QString labelText = tr("Module ") + QString::number(i);
        QColor color = cx_colors::getModuleColor(i);
        Label* label = new Label(labelText, color, false);
        labelWidget->addLabel(label);
        moduleLabels.push_back(label);
    }

    foreach(Node* nativeNode, cppnWidget->getCppn()->getNodes()){
        size_t module = nativeNode->getModule();
        nativeNode->setLabel(moduleLabels[module]);
    }
}


void Window::askRootDir(){
    dbg::trace trace("window", DBG_HERE);
    if(!globalSettings->getDownloadPrefSet()){
    	GenomeDirWidget* genomeDirWidget = new GenomeDirWidget();
    	genomeDirWidget->show();
    	genomeDirWidget->exec();
        delete genomeDirWidget;
    }
//
//    QString msg = tr("Select genome directory");
//    QString dir = QFileDialog::getExistingDirectory(this, msg);
//	globalSettings->setGenomeDir(dir);
}


void Window::loadByID(){
    dbg::trace trace("window", DBG_HERE);
	if(not(askSaveChanges())) return;
	askRootDir();
	if(!globalSettings->getDownloadPrefSet()) return;
	if(globalSettings->getGenomeDir().isEmpty()) return;

	//Get branch id
	bool ok = true;
	QString labelText = "";
	if(_currentBranch > -1){
	    labelText += tr("Current branch: ") +
	    		util::toQString(_currentBranch) + ". ";
	}

    if(_parent > -1){
        labelText += tr("Parent branch: ") + util::toQString(_parent) + ".";
    }

    QList<QString> branches= _getAvailableBranches();
	QString branchNrstr =  QInputDialog::getItem(this,
			tr("Select branch"),
			labelText, branches, 0, true, &ok);
	if(!ok) return;
	int branchNr = branchNrstr.toInt(&ok);
	if(!ok) return;
	_loadBranch(branchNr);
}


QString Window::_getMainFile(int branchNr){
    QString genDir = globalSettings->getGenomeDir();
    QString branchNrStr = util::toQString(branchNr);
	if(branchNr == -1){
		QString message(QString("Invalid branch: ") + branchNrStr);
		QMessageBox::information(this, tr("Invalid branch"), message);
		return QString("");
	}

	QString directory = genDir + "/" + branchNrStr;
	QFileInfo dirInfo(directory);
	QString mainFile = directory + "/main.zip";
	QFileInfo fileInfo(mainFile);

	if(!dirInfo.exists()){
		if(globalSettings->getDownloadPref()){
			if(!QDir().mkpath(directory)){
				QString message = tr("Could create directory:\n") + directory;
				QMessageBox::information(this, message, message);
				return QString("");
			}
		} else {
			QString message = tr("Directory does not exist: ") + directory;
			QMessageBox::information(this, message, message);
			return QString("");
		}
	}

	if(!fileInfo.exists()){
		if(globalSettings->getDownloadPref()){
			QString genUrl = url + "/" + branchNrStr + "/main.zip";
			DownloadAssistant da(genUrl, mainFile);
			if(!da.download()){
				QString message = tr("Could not download file:\n") + genUrl;
				QMessageBox::information(this, message, message);
				return QString("");
			}
		} else {
			QString message = tr("File does not exist: ") + mainFile;
			QMessageBox::information(this, message, message);
			return QString("");
		}
	}

	return mainFile;
}


void Window::_loadBranch(int branchNr, int generation){
    dbg::trace trace("window", DBG_HERE);
    QString mainFile = _getMainFile(branchNr);
    if(mainFile == "") return;
	MainParser parser (mainFile);
	parser.parse();
	_currentBranch = branchNr;
	_parent = parser.parent;
	_maxGeneration = parser.max_generation;
	_storageMap = parser.storage_map;
	if(generation == -1){
		updateGeneration(_maxGeneration);
	} else {
		updateGeneration(generation);
	}

	//Get generation
	QString storageFile = _getStorageFile(_currentGeneration);
	if(storageFile == "") return;
	loadFile(storageFile, _currentGeneration);
}


void Window::loadGeneration(){
    dbg::trace trace("window", DBG_HERE);
	if(!_checkCanSwitchGeneration()) return;

	//Get generation
	bool ok = true;
	int generation =  QInputDialog::getInt(0,
			QInputDialog::tr("Select generation"),
			QInputDialog::tr("Select a generation"),
			_maxGeneration,
			0,
			_maxGeneration,
			1,
			&ok);
	if(!ok){
		return;
	}
	QString storage_file = _getStorageFile(_currentGeneration);
	if(storage_file == "") return;
	updateGeneration(generation);
	loadFile(storage_file, _currentGeneration);
}


void Window::nextGeneration(){
    dbg::trace trace("window", DBG_HERE);
	if(!_checkCanSwitchGeneration()) return;

	if(_currentGeneration == -1){
		QString message = tr("Current generation is not set");
		QMessageBox::information(this, message, message);
		return;
	}

	if(_currentGeneration < _maxGeneration){
		_currentGeneration++;
		updateGeneration(_currentGeneration);
		QString storageFile = _getStorageFile(_currentGeneration);
		if(storageFile == "") return;
		loadFile(storageFile, _currentGeneration);
	} else if(!_branchHistory.empty()){
		int child = _branchHistory.back();
		_branchHistory.pop_back();
		_loadBranch(child, 0);
	}
}


void Window::previousGeneration(){
    dbg::trace trace("window", DBG_HERE);
	if(!_checkCanSwitchGeneration()) return;

	if(_currentGeneration == -1){
		QString message = tr("Current generation is not set");
		QMessageBox::information(this, message, message);
		return;
	}

	if(_currentGeneration > 0){
		_currentGeneration--;
		updateGeneration(_currentGeneration);
		QString storageFile = _getStorageFile(_currentGeneration);
		if(storageFile == "") return;
		loadFile(storageFile, _currentGeneration);
	} else {
		_branchHistory.push_back(_currentBranch);
		_loadBranch(_parent);
	}
}



QString Window::_getStorageFile(int generation){
    dbg::trace trace("window", DBG_HERE);
	int storage = _storageMap[generation];
	QString fileName = util::toQString(storage) + ".zip";
	QString dirName = util::toQString(_currentBranch);
	QString genDir = globalSettings->getGenomeDir();
	QString filePath = genDir + "/" + dirName + "/" + fileName;
	QFileInfo fileInfo (filePath);
	if(!fileInfo.exists()){
		if(globalSettings->getDownloadPref()){
			QString genUrl = url + "/" + dirName + "/" + fileName;
			DownloadAssistant da(genUrl, filePath);
			if(!da.download()){
				QString message = tr("Could not download file:\n") + genUrl;
				QMessageBox::information(this, message, message);
				return QString("");
			}
		} else {
			QString message = tr("File does not exist:\n") + filePath;
			QMessageBox::information(this, message, message);
			return QString("");
		}
	}
	return filePath;
}


bool Window::_checkCanSwitchGeneration(){
    dbg::trace trace("window", DBG_HERE);
	if(globalSettings->getGenomeDir().isEmpty()){
		QString message = tr("Genome dir is not set");
		QMessageBox::information(this, message, message);
		return false;
	}

	if(_currentBranch == -1 ){
		QString message = tr("Current branch is not set");
		QMessageBox::information(this, message, message);
		return false;
	}

	if(_maxGeneration == -1){
		QString message = tr("Max generation is not set");
		QMessageBox::information(this, message, message);
		return false;
	}

	return true;
}


/***
 * Highly experimental
 */
void Window::startAnalysis(){
    dbg::trace trace("analysis", DBG_HERE);
    if(globalSettings->getGenomeDir().isEmpty()) askRootDir();
    if(globalSettings->getGenomeDir().isEmpty()) return;
    analysis::startAnalysis(globalSettings->getGenomeDir());
}


void Window::addConMut(){
    dbg::trace trace("window", DBG_HERE);
    Edge* edge = analysis::addConMut(cppnWidget->getCppn());
    if(edge) cppnWidget->addEdge(edge);
}


void Window::addNodeMut(){
    dbg::trace trace("window", DBG_HERE);
    QPair<QList<Edge*>, Node*> nodeAndEdges;
	nodeAndEdges = analysis::spliceNodeMut(cppnWidget->getCppn());
    cppnWidget->removeEdge(nodeAndEdges.first[0]);
    cppnWidget->addNode(nodeAndEdges.second);
    cppnWidget->addEdge(nodeAndEdges.first[1]);
    cppnWidget->addEdge(nodeAndEdges.first[2]);
}


void Window::save(){
    dbg::trace trace("window", DBG_HERE);
	if(fileInformation->fileName.isEmpty() || fileInformation->newFile){
		saveAs();
	} else {
		actualSave(fileInformation->fileName);
	}
}


void Window::saveAs(){
    dbg::trace trace("window", DBG_HERE);
	QString newFileName = QFileDialog::getSaveFileName(this,
	         tr("Save Genome File"), "",
	         tr("XML File (*.xml);;All Files (*)"));
	actualSave(newFileName);
}


void Window::filmEveryConnection(){
#ifdef USE_FFMPEG
	Cppn* cppn = cppnWidget->getCppn();
	qreal incre=0.05;
	QString msg = tr("Chose save directory");
	QString dir = QFileDialog::getExistingDirectory(this, msg);
	for(size_t i=0; i<cppn->getNrOfEdges(); ++i){
	//for(size_t i=0; i<1; ++i){
		Edge* edge = cppn->getEdge(i);
		QVideoEncoder encoder;
		QString filepath = dir + "/" + "edge" + edge->getNameQ() + ".mpg";
		if(!encoder.createFile(filepath, 256, 256, 262144, 10, 30)){
			std::cout << "Error while creating file." << std::endl;
			return;
		}
		for(qreal w=edge->getWeight(); w>=-3; w-=incre){
			edge->setWeight(w, true);
			QImage img = nodeviewWidget->getFinalNodeView()->getImage()->copy(0,0,256,256);
			encoder.encodeImage(img);
		}
		for(qreal w=-3.0; w<=3.0; w+=incre){
			edge->setWeight(w, true);
			QImage img = nodeviewWidget->getFinalNodeView()->getImage()->copy(0,0,256,256);
			encoder.encodeImage(img);
		}
		for(qreal w=3.0; w>=edge->getOriginalWeight(); w-=incre){
			edge->setWeight(w, true);
			QImage img = nodeviewWidget->getFinalNodeView()->getImage()->copy(0,0,256,256);
			encoder.encodeImage(img);
		}
		if(!encoder.close()){
			std::cout << "Error while closing the encoder." << std::endl;
			return;
		}
		edge->setWeight(edge->getOriginalWeight(), true);
	}
#endif //USE_FFMPEG
}


//Scan and screen capture
void Window::startFilm(){
    dbg::trace trace("window", DBG_HERE);
    QString msg = tr("Chose save directory");
	QString newFileName = util::getSupportedFilename(this, msg);
#ifdef USE_FFMPEG
	bool success = false;
#endif //USE_FFMPEG
	if(newFileName.isEmpty()) return;

	captureDirectory = util::getBase(newFileName);
	extention = util::getExtention(newFileName);
	QDir().mkdir(captureDirectory);

	nodeViewsToBeCaptured.clear();

	if(this->isMaximized()) this->move(0,0);
	int width = this->width()-this->width()%16;
	int height = this->height()-this->height()%16;
	this->setFixedSize(width, height);

	QDir().mkdir(captureDirectory + "/fullApplication");

#ifdef USE_FFMPEG
	nodeViewEncoders.clear();
	success = encoder.createFile(captureDirectory + "/fullApplication.mpg",
			this->width(),
			this->height(),this->width()*this->height()*4,10, 30);
	if(!success) return;
#endif //USE_FFMPEG

	//Select the final nodeview
	nodeviewWidget->getFinalNodeView()->setSelected(true);
//	QList<QGraphicsItem*> allNodeviews = nodeviewWidget->scene()->items();
//	foreach(QGraphicsItem* item, allNodeviews){
////		std::cout << "Searching for finalnodeview..." << std::endl;
//		FinalNodeView* finalnodeView = qgraphicsitem_cast<FinalNodeView*>(item);
//		if(finalnodeView){
////			std::cout << "Final nodeview found!" << std::endl;
//			finalnodeView->setSelected(true);
//		}
//	}


	QList<QGraphicsItem*> gi = nodeviewWidget->scene()->selectedItems();
	foreach(QGraphicsItem* item, gi){
		NodeView* nodeView = util::multiCast<NodeView*, FinalNodeView*>(item);
		if(nodeView){
			nodeViewsToBeCaptured.append(nodeView);
			QString nbStr = util::toQString(nodeViewsToBeCaptured.size());
			QDir().mkdir(captureDirectory + "/node" + nbStr);

#ifdef USE_FFMPEG
			QVideoEncoder* nodeViewEncoder = new QVideoEncoder();
			success = nodeViewEncoder->createFile(captureDirectory + "/node" +
					util::toQString(nodeViewsToBeCaptured.size()) + ".mpg",
					256, 256, 262144, 10, 30);
			if(!success) return;
			nodeViewEncoders.append(nodeViewEncoder);
#endif //USE_FFMPEG
		}
	}

	frame=0;
	weightWidget->startFilm();
}


void Window::stopFilm(){
    dbg::trace trace("window", DBG_HERE);
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
#endif //USE_FFMPEG
}


void Window::captureFrame(){
    dbg::trace trace("window", DBG_HERE);
	QPixmap pm = QPixmap::grabWidget(this, 0, 0, this->width(), this->height());
	QString name = captureDirectory + "/fullApplication/frame%1" + extention;
	name = name.arg(util::toQString(frame), 4, '0');
	pm.save(name);

#ifdef USE_FFMPEG
	QImage image = pm.toImage();
	image = image.convertToFormat(QImage::Format_RGB32);
	encoder.encodeImage(image);
#endif //USE_FFMPEG

	for(int i=0; i<nodeViewsToBeCaptured.size(); i++){
		QImage img = nodeViewsToBeCaptured[i]->getImage()->copy(0,0,256,256);
		QString name = captureDirectory + "/node%1/frame%2" + extention;
		name = name.arg(util::toQString(i+1));
		name = name.arg(util::toQString(frame), 4, '0');
		img.save(name);

#ifdef USE_FFMPEG
		nodeViewEncoders[i]->encodeImage(img);
#endif //USE_FFMPEG
	}
	frame++;
}


void Window::startScan(){
    dbg::trace trace("window", DBG_HERE);
	cppnWidget->setInteractive(false);
	nodeviewWidget->setInteractive(false);
	labelWidget->setEnabled(false);
}


void Window::stopScan(){
    dbg::trace trace("window", DBG_HERE);
	cppnWidget->setInteractive(true);
	nodeviewWidget->setInteractive(true);
	labelWidget->setEnabled(true);
}


void Window::exportToImageSeries(){
    dbg::trace trace("window", DBG_HERE);
    Edge* edge = cppnWidget->getSelectedEdge();
    if(!edge){
        DBOW << "exportToImageSeries: no edge selected, return;" << std::endl;
        return;
    }

    QString fn = util::getSupportedFilename(this, tr("Chose save directory"));
    if(fn.isEmpty()){
    	DBOW << "exportToImageSeries: file name is empty, return;" << std::endl;
        return;
    }

    QString baseName = util::getBase(fn);
    QString folderStartName = util::getNameFromBase(baseName);
    QString edgeName = QString::fromStdString(edge->getId()) + "_" +
    		QString::fromStdString(edge->getBranch());
    QString directory = baseName + "_" + edgeName;

    extention = util::getExtention(fn);
    QDir().mkdir(directory);

    double originalWeight = weightWidget->getValue();
    double start = weightWidget->getScanStartValue();
    double stop = weightWidget->getScanStopValue();
    double step = weightWidget->getStepSizeValue();
    int imageNumber = 1;

    DBOW << "Start: " << start << " stop: " << stop <<
    		" step: " << step << std::endl;
    for(double weight=start; weight<=stop; weight+=step){
    	DBOW << "Creating image for weight: " << weight << std::endl;
        cppnWidget->setWeight(weight);
        QSharedPointer<QImage> img;
        img = nodeviewWidget->getFinalNodeView()->getImage();
        QImage nodeViewImage = img->copy(0,0,256,256);
        QString imageNbStr = QString::number(imageNumber);
        QString weightStr = _formatWeight(weight);
        QString name = directory + folderStartName + "_" + imageNbStr +
        		"_" + edgeName + "_" + weightStr + extention;
        DBOW << "Saving image as: " << name.toStdString() << std::endl;

        nodeViewImage.save(name);
        ++imageNumber;
    }
    cppnWidget->setWeight(originalWeight);
}


QString Window::_formatWeight(double weight){
    dbg::trace trace("window", DBG_HERE);
    int precision = 2;
    QString result = "+";;
    QString weightStr = QString::number(weight, 'f', precision);
    int strIndex = 0;
    if(weightStr[strIndex] == '-'){
        result = "-";
        ++strIndex;
    }

    result += QChar(weightStr[strIndex]);
    result += "_";
    strIndex+=2;

    for(int i=0; i<precision; ++i){
        if(strIndex < weightStr.size()){
            result+=QChar(weightStr[strIndex]);
            strIndex++;
        } else {
            result+='0';
        }
    }

    return result;
}


void Window::executeCommand(ComBase* command){
    dbg::trace trace("window", DBG_HERE);
	if(command->isOk()){
		undoStack.push(command);
	} else {
		delete command;
	}
}


void Window::createNullModel(){
	dbg::trace trace("window", DBG_HERE);
	QString genDir = globalSettings->getGenomeDir();
	DBOW << "Retrieving parent CPPN" << std::endl;
	Cppn* parCppn = analysis::getBranchCppn(genDir, _currentBranch, 0);
	Cppn* origCppn = cppnWidget->getCppn();
	Cppn* nullM;
	bool color = origCppn->getOutputs().size() > 1;
	size_t nbNodes = origCppn->getNrOfNodes();
	size_t nbEdges = origCppn->getNrOfEdges();
	DBOW << "Creating null-model" << std::endl;
	nullM = analysis::randCppnNeatMutPar(nbNodes, nbEdges, color, parCppn);
	DBOW << "Setting null-model as CPPN" << std::endl;
	cppnWidget->setCppn(nullM);
	cppnWidget->positionNodesLayers();

    QList<NodeView*> nodeviews;
    FinalNodeView* finalNodeview = new FinalNodeView();
    foreach(Node* node, nullM->getOutputs()){
    	if(node->getXmlLabel() == OUTPUT_INK ||
    			node->getXmlLabel() == OUTPUT_BRIGTHNESS)
    	{
    		finalNodeview->setValueNode(node);
    	}
    	if(node->getXmlLabel() == OUTPUT_SATURATION){
    		finalNodeview->setSaturationNode(node);
    	}
    	if(node->getXmlLabel() == OUTPUT_HUE){
    		finalNodeview->setHueNode(node);
    	}
    }
    finalNodeview->update();
    nodeviews.push_back(finalNodeview);
    nodeviewWidget->setNodeviews(nodeviews);
}

/*********************************************
 ************* PRIVATE FUNCTIONS *************
 *********************************************/

void Window::actualSave(const QString& fileName){
    dbg::trace trace("window", DBG_HERE);
	if(fileName.isEmpty()) return;
	try{
		CppnWriter writer(fileName.toStdString());
		writer.write(cppnWidget->getCppn(), labelWidget->getLabels(),
				fileInformation, nodeviewWidget->getNodeviews());
		fileInformation->newFile = false;
		fileInformation->fileName = fileName;
		setWindowModified(false);
		setWindowTitle(fileInformation->fileName.split('/').back() + "[*]");
		undoStack.setClean();

	} catch(std::exception& e){
		QString message(("Error saving file: " + fileName.toStdString() +
				"\n" + std::string( e.what() )).c_str());
		QMessageBox::information(this, tr("Unable to open file"), message);
	}
}

bool Window::askSaveChanges(){
    dbg::trace trace("window", DBG_HERE);
	if(isWindowModified ()){
		 QString msg = "The file " + fileInformation->fileName +
				 " has been modified.";
		 QFlags<enum QMessageBox::StandardButton> btns;
		 btns = QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel;
		 QMessageBox msgBox(QMessageBox::Question,tr("Save as"), msg, btns,
				 this, Qt::Dialog);
		 msgBox.setWindowModality ( Qt::WindowModal);

		 msgBox.setText(msg);
		 msgBox.setInformativeText("Do you want to save your changes?");
		 msgBox.setStandardButtons(btns);
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
    dbg::trace trace("window", DBG_HERE);
	event->setAccepted(askSaveChanges());
	if(event->isAccepted()){
	    saveSettings();
	}
}


void Window::fileLoaded(bool selected){
    dbg::trace trace("window", DBG_HERE);
	saveAction->setEnabled(selected);
	saveAsAction->setEnabled(selected);
	_nextGeneration->setEnabled(selected);
	_previousGeneration->setEnabled(selected);
	_exportToImageSeriesAction->setEnabled(selected);
	_createModuleLabelsAction->setEnabled(selected);
	_compareGenomesAction->setEnabled(selected);
	_importPositionsAction->setEnabled(selected);
	_importLabelsAction->setEnabled(selected);

	labelWidget->setEnabled(selected);
	cppnWidget->setEnabled(selected);
	weightWidget->setEnabled(selected);
	nodeviewWidget->setEnabled(selected);
}

void Window::onCleanState(bool state){
    dbg::trace trace("window", DBG_HERE);
	setWindowModified(!state);
}

//Legend
void Window::toggleLegend(){
    dbg::trace trace("window", DBG_HERE);
    if(!cppnWidget->isEnabled()) return;
    if(!cppnWidget->getLegend()){
        cppnWidget->addLegend(labelWidget->getLabels());
    } else {
        cppnWidget->removeLegend();
    }
}

void Window::selectAll(){
    dbg::trace trace("window", DBG_HERE);
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
    dbg::trace trace("window", DBG_HERE);
	QWidget* widget =  QApplication::focusWidget();
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
		DragAndDropGraphicsView* dadGraphicsView;
		dadGraphicsView = qobject_cast<DragAndDropGraphicsView*> (widget);
		if(dadGraphicsView){
			labelWidget->requestDelete();
			return;
		}
	}
}


void Window::exportToJpg(){
    dbg::trace trace("window", DBG_HERE);
	if(nodeviewWidget->getIsActive()){
		nodeviewWidget->saveImage();
	} else if(cppnWidget->getIsActive()){
		cppnWidget->saveImage();
	}
}


void Window::onSelectionChanged(){
    dbg::trace trace("window", DBG_HERE);
	bool nodeViewSelected = (nodeviewWidget->getIsActive() &&
			nodeviewWidget->scene()->selectedItems().count() > 0);
	bool deletableNodeviewSelected = (nodeviewWidget->getIsActive() &&
			nodeviewWidget->deletableNodeviewSelected());

	bool nodeSelected = (cppnWidget->getIsActive() &&
			cppnWidget->getNodeSelected());
	bool edgeSelected = (cppnWidget->getIsActive() &&
			cppnWidget->getEdgeSelected());
	bool labelSelected = (labelWidget->getGraphicsViewHasFocus() &&
			labelWidget->getLabelSelected());

	exportImageAction->setEnabled(nodeViewSelected || nodeSelected);
	removeAction->setEnabled(deletableNodeviewSelected || nodeSelected ||
			edgeSelected || labelSelected);
	selectAllAction->setEnabled(nodeviewWidget->hasFocus() ||
			cppnWidget->hasFocus() || labelWidget->getGraphicsViewHasFocus());
}


void Window::updateModularity(double qscore){
    dbg::trace trace("window", DBG_HERE);
	_qscore = qscore;
	_updateStatusText();
}


void Window::updateGeneration(int generation){
    dbg::trace trace("window", DBG_HERE);
	_currentGeneration = generation;
	_updateStatusText();
}


void Window::_updateStatusText(){
    dbg::trace trace("window", DBG_HERE);
	modularityLabel->setText(modularityText + util::toQString(_qscore) +
			QString(" ") + branchText + util::toQString(_currentBranch) +
			QString(" ") + generationText + util::toQString(_currentGeneration)
	);
}

//Create action
QAction* Window::_createAction(QString name, QString tip, const char *member){
    dbg::trace trace("window", DBG_HERE);
    QAction* action = new QAction(name, this);
    action->setStatusTip(tip);
    connect(action, SIGNAL(triggered()), this, member);
    addAction(action);
    return action;
}

void Window::preferences(){
    PreferencesWidget* preferences = new PreferencesWidget();
    preferences->show();
    preferences->exec();
    //delete preferences;
}

bool sortByNumber(const QString &s1, const QString &s2)
{
	bool ok1, ok2;
	int i1 = s1.toInt(&ok1);
	int i2 = s2.toInt(&ok2);
	if(ok1 && !ok2) return true;
	if(!ok1 && ok2) return false;
	if(ok1 && ok2) return i1 < i2;
    return s1 < s2;
}

QList<QString> Window::_getAvailableBranches(){
	QList<QString> result;
    QString genDir = globalSettings->getGenomeDir();
	QFileInfo dirInfo(genDir);
	QString branchFilePath = genDir + "/branches.txt";
	QFileInfo fileInfo(branchFilePath);

	 if(!fileInfo.exists() && globalSettings->getDownloadPref()){
			QString genUrl = url + "/branches.txt";
			DownloadAssistant da(genUrl, branchFilePath);
			da.download();
			fileInfo.refresh();
	 }

	if(fileInfo.exists()){
		// Read branch file
		QFile branchFile (branchFilePath);
	    if (branchFile.open(QIODevice::ReadOnly)){
	    	while(!branchFile.atEnd()){
	    		QString str = branchFile.readLine();
	    		result.append(str.trimmed());
	    	}
	    	return result;
	    } else {
	    	std::cout << "Failed to open file: " <<
	    			branchFilePath.toStdString() << std::endl;
	    }
	}

	QDir dir;
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	dir.setPath(genDir);
	for (uint i = 0; i < dir.count(); ++i){
		result.append(dir[i]);
	}
	qSort(result.begin(), result.end(), sortByNumber);
	QFile branchFile (branchFilePath);
	if (!branchFile.open(QIODevice::WriteOnly)){
		return result;
	}
	for (int i = 0; i < result.size(); ++i){
		branchFile.write(result[i].toUtf8());
		branchFile.write("\n");
	}

	return  result;
}
