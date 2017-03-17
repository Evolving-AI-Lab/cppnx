/*
 * window.cpp
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */

//QT includes
#include <QSettings>

//Local includes
#include "CE_Window.h"
#include "CE_Util.h"
#include "CX_ModuleColor.h"
#include "CX_Random.hpp"
#include "CX_Hierarchy.hpp"
#include "CX_Debug.hpp"
#include "CX_PreferencesWidget.hpp"

//Standard includes
#include <cmath>
#include <time.h>

const QString Window::modularityText = "Modularity (Q-score): ";
const QString Window::hierachyText = "Hierarchy: ";
const QString Window::generationText = "Generation: ";

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
//    connect(cppnWidget, SIGNAL(sceneModified()), this, SLOT(onSceneModified()));
    connect(cppnWidget, SIGNAL(requestCommandExecution(ComBase*)), this, SLOT(executeCommand(ComBase*)));
    connect(cppnWidget, SIGNAL(edgeUpdated(Edge*)), this, SLOT(updateEdge(Edge*)));
    connect(cppnWidget, SIGNAL(nodeUpdated(Node*)), this, SLOT(updateNode(Node*)));
    connect(cppnWidget, SIGNAL(newModularity(double)), this, SLOT(updateModularity(double)));
    connect(cppnWidget, SIGNAL(labelableObjectSelected(bool)), labelWidget, SLOT(labelableObjectSelected(bool)));
    connect(cppnWidget, SIGNAL(requestAddNodeview(QList<QGraphicsItem*>)), nodeviewWidget, SLOT(addNodeView(QList<QGraphicsItem*>)));
    connect(cppnWidget->scene(), SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(cppnWidget, SIGNAL(focusChanged()), this, SLOT(onSelectionChanged()));

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
    connect(nodeviewWidget, SIGNAL(focusChanged()), this, SLOT(onSelectionChanged()));
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
    _nextGeneration->setStatusTip(tr("If the current genome was loaded by ID, go to the next generation."));
    connect(_nextGeneration, SIGNAL(triggered()), this, SLOT(nextGeneration()));

    _previousGeneration = new QAction(tr("Previous generation"), this);
    _previousGeneration->setShortcut(tr("Ctrl+<"));
    _previousGeneration->setStatusTip(tr("If the current genome was loaded by ID, go to the previous generation."));
    connect(_previousGeneration, SIGNAL(triggered()), this, SLOT(previousGeneration()));

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
    exportImageAction->setStatusTip(tr("Exports the selected nodes as image files"));
    exportImageAction->setEnabled(false);
    connect(exportImageAction, SIGNAL(triggered()), this, SLOT(exportToJpg()));

    _startAnalysisAction = new QAction(tr("&Start analysis"), this);
    _startAnalysisAction->setStatusTip(tr("Starts the analysis of a folder. WARNING: Long and experimental procedure!"));
    _startAnalysisAction->setEnabled(true);
    connect(_startAnalysisAction, SIGNAL(triggered()), this, SLOT(startAnalysis()));

    _compareGenomesAction = new QAction(tr("&Mark differences..."), this);
    _compareGenomesAction->setStatusTip(tr("Marks the nodes that are not in the chosen genome."));
    _compareGenomesAction->setEnabled(true);
    connect(_compareGenomesAction, SIGNAL(triggered()), this, SLOT(markDifferences()));

    _importPositionsAction = new QAction(tr("Import positions..."), this);
    _importPositionsAction->setStatusTip(tr("Import the positions from another genome file."));
    _importPositionsAction->setEnabled(true);
    connect(_importPositionsAction, SIGNAL(triggered()), this, SLOT(importPositions()));

    _importLabelsAction = new QAction(tr("Import labels..."), this);
    _importLabelsAction->setStatusTip(tr("Import the labels from another genome file."));
    _importLabelsAction->setEnabled(true);
    connect(_importLabelsAction, SIGNAL(triggered()), this, SLOT(importLabels()));

    _createModuleLabelsAction = new QAction(tr("Create module labels"), this);
    _createModuleLabelsAction->setStatusTip(tr("Create labels based on the modules in the genome. Overwrites current labels."));
    _createModuleLabelsAction->setEnabled(true);
    connect(_createModuleLabelsAction, SIGNAL(triggered()), this, SLOT(createModuleLabels()));

    _toggleLegendAction = _createAction(tr("Toggle legend"), tr("Toggles whether a legend is shown."), SLOT(toggleLegend()));
    _toggleLegendAction->setCheckable(true);
    cppnWidget->addAction(_toggleLegendAction);

    _exportToImageSeriesAction = _createAction(tr("Export to image series"),
            tr("Scan over the selected connection and export the resulting images"),
            SLOT(exportToImageSeries()));

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
    //File menu
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(exitAction);
    fileMenu->addAction(_newAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(loadIDAction);
    fileMenu->addAction(_nextGeneration);
    fileMenu->addAction(_previousGeneration);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(_preferencesAction);

    //Position menu
    posMenu = new QMenu(tr("&Position"), this);
    posMenu->addAction(cppnWidget->getCircleAction());
    posMenu->addAction(cppnWidget->getONPAction());
    posMenu->addAction(cppnWidget->getLayerAction());

    //Edit menu
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

    //View menu
    viewMenu= new QMenu(tr("&View"), this);
    viewMenu->addAction(cppnWidget->getEdgeShowLabelAction());
    viewMenu->addAction(cppnWidget->getEdgeNoLabelAction());
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
    viewMenu->addAction(cppnWidget->getToggleAnnotationsAction());
    viewMenu->addSeparator();
    viewMenu->addAction(cppnWidget->getZoomInAction());
    viewMenu->addAction(cppnWidget->getZoomOutAction());
    viewMenu->addAction(cppnWidget->getSetZoomAction());
    viewMenu->addSeparator();


    //Search menu
    _searchMenu = new QMenu(tr("&Search"), this);
    _searchMenu->addAction(cppnWidget->getFindUnlabeledEdgeAction());
    _searchMenu->addAction(cppnWidget->getFindEdgeByIdAction());

    //Experimental menu
    _experimentalMenu= new QMenu(tr("&Experimental"), this);
    _experimentalMenu->addAction(cppnWidget->getIncreaseCurveOffsetAction());
    _experimentalMenu->addAction(cppnWidget->getDecreaseCurveOffsetAction());
    _experimentalMenu->addAction(cppnWidget->getSnapshotAction());
    _experimentalMenu->addAction(_startAnalysisAction);
    _experimentalMenu->addAction(_compareGenomesAction);
    _experimentalMenu->addAction(_importPositionsAction);
    _experimentalMenu->addAction(_importLabelsAction);
    _experimentalMenu->addAction(_createModuleLabelsAction);
    _experimentalMenu->addAction(cppnWidget->getSetSinAction());
    _experimentalMenu->addAction(cppnWidget->getSetCosAction());
    _experimentalMenu->addAction(cppnWidget->getSetGausAction());
    _experimentalMenu->addAction(cppnWidget->getSetSigmoidAction());
    _experimentalMenu->addAction(cppnWidget->getSetLinAction());
    _experimentalMenu->addAction(cppnWidget->getSetStepAction());
    _experimentalMenu->addAction(cppnWidget->getSetUGaussianAction());
    _experimentalMenu->addAction(cppnWidget->getSetUSigmoidAction());
    _experimentalMenu->addAction(cppnWidget->getSetUBoundedLinearAction());
    _experimentalMenu->addAction(cppnWidget->getSetStepAction());
    _experimentalMenu->addAction(cppnWidget->getAddNodeAction());
    _experimentalMenu->addAction(cppnWidget->getAddNodeOnConnectionAction());
    _experimentalMenu->addAction(cppnWidget->getAddEdgeAction());


    _experimentalMenu->addAction(cppnWidget->getAlignHorizontalAction());
    _experimentalMenu->addAction(cppnWidget->getAlignVerticalAction());
    _experimentalMenu->addAction(cppnWidget->getSpaceHorizontalAction());
    _experimentalMenu->addAction(cppnWidget->getSpaceVerticalAction());



    _experimentalMenu->addAction(cppnWidget->getScaleLegendAction());

    _experimentalMenu->addAction(_exportToImageSeriesAction);
    _experimentalMenu->addAction(cppnWidget->getColorPathAction());

//    _experimentalMenu->addAction(cppnWidget->getSetFavoriteAction());


    //Menu bar
    menuBar = new QMenuBar;
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(viewMenu);
    menuBar->addMenu(posMenu);
    menuBar->addMenu(_searchMenu);
    menuBar->addMenu(_experimentalMenu);
//    menuBar->addAction(preferences);

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

//    _genomeDir = tr("");
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
	         tr("Genome file (*.xml;*.zip);;All Files (*)"));
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
		CppnParser parser(filename.toStdString());
		parser.parse(generation);

		dbg::out(dbg::info, "window") << "Loading cppn" << std::endl;
		if(cppnWidget->getLegend()){
		    cppnWidget->removeLegend();
		    _toggleLegendAction->setChecked(false);
		}
		cppnWidget->setCppn(parser.takeNodes(), parser.takeEdges());

		dbg::out(dbg::info, "window") << "Loading labels" << std::endl;
		labelWidget->setLabels(parser.getLabels());

		dbg::out(dbg::info, "window") << "Loading nodeviews" << std::endl;
		nodeviewWidget->setNodeviews(parser.takeNodeviews());

		dbg::out(dbg::info, "window") << "Loading file-information" << std::endl;
		fileInformation = parser.takeFileInformation();

		dbg::out(dbg::info, "window") << "Positioning nodes" << std::endl;
		if(fileInformation->newFile) cppnWidget->positionNodesLayers();

		undoStack.clear();
		fileInformation->fileName = filename;
		fileLoaded(true);

		setWindowTitle(filename.split('/').back() +"[*]");
		setWindowModified(false);

		dbg::out(dbg::info, "window") << "Load done" << std::endl;
	} catch(std::exception& e){
		QString message(("Error reading file: " + filename.toStdString() + "\n" + std::string( e.what() )).c_str());
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
//    QSettings settings(_settingsFile, QSettings::IniFormat);
//    _genomeDir = settings.value("directories/genome", "").toString();
}

void Window::saveSettings(){
    dbg::trace trace("window", DBG_HERE);
    globalSettings->save();
//    QSettings settings(_settingsFile, QSettings::IniFormat);
//    settings.setValue("directories/genome", _genomeDir);
}


void Window::_markDifferences(QString filename, int generation){
    dbg::trace trace("window", DBG_HERE);
    try{
        CppnParser parser(filename.toStdString());
        parser.parse(generation);

        Label* newLabel = new Label("New", QColor(200, 200, 0), false);
        Label* oldLabel = new Label("Unchanged", QColor(0, 0, 200), false);
        Label* weightChangedLabel = new Label("Weight changed", QColor(0, 200, 0), false);

        labelWidget->clear();
        labelWidget->addLabel(newLabel);
        labelWidget->addLabel(oldLabel);
        labelWidget->addLabel(weightChangedLabel);

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
                        nativeEdge->setLabel(weightChangedLabel);
                    }

                    found = true;
                }
            }
            if(!found){
                nativeEdge->setLabel(newLabel);
            }
        }
    } catch(std::exception& e){
        QString message(("Error reading file: " + filename.toStdString() + "\n" + std::string( e.what() )).c_str());
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
        QString message(("Error reading file: " + filename.toStdString() + "\n" + std::string( e.what() )).c_str());
        QMessageBox::information(this, tr("Unable to open file"), message);
    }
}

void Window::_importLabels(QString filename, int generation){
    dbg::trace trace("window", DBG_HERE);
    CppnParser parser(filename.toStdString());
    try{
        parser.parse(generation);
    } catch(std::exception& e){
        QString message(("Error reading file: " + filename.toStdString() + "\n" + std::string( e.what() )).c_str());
        QMessageBox::information(this, tr("Unable to open file"), message);
        return;
    }

    //Build a command so we can undo this action
    ComBase *importLabelsAction = new ComBase();
    importLabelsAction->setText("import labels");

    CommandAddLabel* removeLabels = new CommandAddLabel(importLabelsAction);
    removeLabels->init(labelWidget, labelWidget->getLabels(), false);

    //To make sure the imported labels are not connected to non-existing edges and nodes,
    //we don't use the labels directly. Instead we make copies of these labels, were the copies
    //will not inherit any qt connections.
    CommandAddLabel* addLabels = new CommandAddLabel(importLabelsAction);
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
                CommandLabelObject* applyLabels = new CommandLabelObject(importLabelsAction);
                applyLabels->init(nativeNode, labelMap[foreignNode->getLabel()]);
            }
        }
    }

    foreach(Edge* nativeEdge, cppnWidget->getCppn()->getEdges()){
        foreach(Edge* foreignEdge, parser.getEdges()){
            if(foreignEdge->getId() == nativeEdge->getId()){
                CommandLabelObject* applyLabels = new CommandLabelObject(importLabelsAction);
                applyLabels->init(nativeEdge, labelMap[foreignEdge->getLabel()]);
            }
        }
    }

#ifdef DBG_ENABLED
    for(int i=0; i<importLabelsAction->childCount(); ++i){
        dbg::out(dbg::info, "window") << importLabelsAction->child(i)->text().toStdString() << std::endl;
    }
#endif

    executeCommand(importLabelsAction);
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
	globalSettings->setGenomeDir(QFileDialog::getExistingDirectory(this, tr("Select genome directory")));
}

void Window::loadByID(){
    dbg::trace trace("window", DBG_HERE);
	if(not(askSaveChanges())) return;
	if(globalSettings->getGenomeDir().isEmpty()) askRootDir();
	if(globalSettings->getGenomeDir().isEmpty()) return;

	//Get branch id
	bool ok = true;
	QString labelText = "";
	if(_currentBranch > -1){
	    labelText += QInputDialog::tr("Current branch: ") + util::toQString(_currentBranch) + ". ";
	}

    if(_parent > -1){
        labelText += QInputDialog::tr("Parent branch: ") + util::toQString(_parent) + ".";
    }

	int branchNr =  QInputDialog::getInt(0,
			QInputDialog::tr("Select branch"),
			labelText,
			0, 0, 99999, 1, &ok);
	if(!ok){
		return;
	}
	_loadBranch(branchNr);
}


void Window::_loadBranch(int branchNr, int generation){
    dbg::trace trace("window", DBG_HERE);
	if(branchNr == -1){
		QString message(QString("Invalid branch: ") + util::toQString(branchNr));
		QMessageBox::information(this, tr("Invalid branch"), message);
		return;
	}

	QString directory = globalSettings->getGenomeDir() + "/" + util::toQString(branchNr);
	QFileInfo dirInfo(directory);
	if(!dirInfo.exists()){
		QString message(QString("Directory does not exist: ") + directory);
		QMessageBox::information(this, tr("Directory does not exist"), message);
		return;
	}

	QString mainFile = directory + "/main.zip";
	QFileInfo fileInfo(mainFile);
	if(!fileInfo.exists()){
		QString message(QString("File does not exist: ") + mainFile);
		QMessageBox::information(this, tr("File does not exist"), message);
		return;
	}

	MainParser parser (mainFile);
	parser.parse();
	_currentBranch = branchNr;
	_parent = parser.parent;
	_maxGeneration = parser.max_generation;
	_storageMap = parser.storage_map;
	if(generation == -1){
		_updateCurrentGeneration(_maxGeneration);
	} else {
		_updateCurrentGeneration(generation);
	}

	//Get generation
	QString storageFile = _getStorageFile(_currentGeneration);
	loadFile(storageFile, _currentGeneration);
}

void Window::loadGeneration(){
    dbg::trace trace("window", DBG_HERE);
	if(!_checkCanSwicthGeneration()) return;

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

	int storage = _storageMap[generation];

	QString storage_file = globalSettings->getGenomeDir() + "/" + util::toQString(_currentBranch) + "/" + util::toQString(storage) + ".zip";
	QFileInfo fileInfo(storage_file);
	if(!fileInfo.exists()){
		QString message(QString("File does not exist: ") + storage_file);
		QMessageBox::information(this, tr("File does not exist"), message);
		return;
	}

	_updateCurrentGeneration(generation);

	loadFile(storage_file, _currentGeneration);
}


void Window::nextGeneration(){
    dbg::trace trace("window", DBG_HERE);
	if(!_checkCanSwicthGeneration()) return;

	if(_currentGeneration == -1){
		QString message(QString("Current generation is not set"));
		QMessageBox::information(this, tr("Current generation is not set"), message);
		return;
	}

	if(_currentGeneration < _maxGeneration){
		_currentGeneration++;
		_updateCurrentGeneration(_currentGeneration);
		QString storageFile = _getStorageFile(_currentGeneration);
		loadFile(storageFile, _currentGeneration);
	} else if(!_branchHistory.empty()){
		int child = _branchHistory.back();
		_branchHistory.pop_back();
		_loadBranch(child, 0);
	}
}


void Window::previousGeneration(){
    dbg::trace trace("window", DBG_HERE);
	if(!_checkCanSwicthGeneration()) return;

	if(_currentGeneration == -1){
		QString message(QString("Current generation is not set"));
		QMessageBox::information(this, tr("Current generation is not set"), message);
		return;
	}

	if(_currentGeneration > 0){
		_currentGeneration--;
		_updateCurrentGeneration(_currentGeneration);
		QString storageFile = _getStorageFile(_currentGeneration);
		loadFile(storageFile, _currentGeneration);
	} else {
		_branchHistory.push_back(_parent);
		_loadBranch(_parent);
	}
}



QString Window::_getStorageFile(int generation){
    dbg::trace trace("window", DBG_HERE);
	int storage = _storageMap[generation];
	return globalSettings->getGenomeDir() + "/" + util::toQString(_currentBranch) + "/" + util::toQString(storage) + ".zip";
}


bool Window::_checkCanSwicthGeneration(){
    dbg::trace trace("window", DBG_HERE);
	if(globalSettings->getGenomeDir().isEmpty()){
		QString message(QString("Genome dir is not set"));
		QMessageBox::information(this, tr("Genome dir is not set"), message);
		return false;
	}

	if(_currentBranch == -1 ){
		QString message(QString("Current branch is not set"));
		QMessageBox::information(this, tr("Current branch is not set"), message);
		return false;
	}

	if(_maxGeneration == -1){
		QString message(QString("Max generation is not set"));
		QMessageBox::information(this, tr("Max generation is not set"), message);
		return false;
	}

	return true;
}


std::vector<double> Window::readNullModels(QString fileName){
    dbg::trace trace("window", DBG_HERE);
    std::vector<double> result;
    double averageModularity = 0;
    double averageForwardHierarchy = 0;
    double averageBackwardHierarchy = 0;
    size_t individuals = 0;

    QFile file(globalSettings->getGenomeDir() + fileName);
    if(file.exists()){
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);

            while(!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(" ");
//                int individualBranch = fields[0].toInt();
                averageModularity += fields[2].toDouble();
                averageForwardHierarchy += fields[3].toDouble();
                averageBackwardHierarchy += fields[4].toDouble();
                individuals++;
            }
            file.close();
        }
    }

    result.push_back(averageModularity/double(individuals));
    result.push_back(averageForwardHierarchy/double(individuals));
    result.push_back(averageBackwardHierarchy/double(individuals));
    return result;
}

void Window::writeBranchingVsMetric(QDir& directory,
        QString fileName,
        std::map<int, double>& map,
        std::map<int, std::vector<int> >& reverseFamilyMap,
        double correction){
    dbg::trace trace("window", DBG_HERE);
    QFile file(globalSettings->getGenomeDir() + fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream file_out(&file);
    for (uint i = 0; i < directory.count(); ++i){
        int currentBranch = util::toInt(directory[i].toStdString());
        if(map.count(currentBranch)){
            double metric = map[currentBranch] - correction;
            size_t branching = 0;
            if(reverseFamilyMap.count(currentBranch)){
                branching = reverseFamilyMap[currentBranch].size();
            }
            file_out << branching << " " << metric << "\n";
        }
    }
    file.close();
}

/***
 * Highly experimental
 */
void Window::startAnalysis(){
    dbg::trace trace("analysis", DBG_HERE);
    bool image = false;
    if(globalSettings->getGenomeDir().isEmpty()) askRootDir();
    QDir directory;
    directory.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    directory.setPath(globalSettings->getGenomeDir());
    std::cout << "Paths found: " << directory.count() << std::endl;

    //Build the family map, modularity map and reverse family maps based on the genome data
    std::map<int, int> familyMap;
    std::map<int, double> modularityMap;
    std::map<int, double> forwardHierarchyMap;
    std::map<int, double> backwardHierarchyMap;
    std::map<int, std::vector<int> > reverseFamilyMap;

    QList<int> parentPointToSelf;

    QFile file(globalSettings->getGenomeDir() + "/genome_data.dat");
    if(file.exists()){
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);

            while(!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(" ");
                int child = fields[0].toInt();
                int parent = fields[1].toInt();
                if (child == parent){
                    parentPointToSelf.append(child);
                }
                double modularity = fields[2].toDouble();
                double forwardHierarchy = fields[3].toDouble();
                double backwardHierarchy = fields[4].toDouble();
                familyMap[child] = parent;
                modularityMap[child] = modularity;
                forwardHierarchyMap[child] = forwardHierarchy;
                backwardHierarchyMap[child] = backwardHierarchy;
                if(!reverseFamilyMap.count(parent)){
                    reverseFamilyMap[parent] = std::vector<int>();
                }
                reverseFamilyMap[parent].push_back(child);
            }
            file.close();
        }
    }

    //Log broken dependencies
    QFile brokenDependenciesFile(globalSettings->getGenomeDir() + "/broken_depencies.dat");
    brokenDependenciesFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream brokenDependenciesFileStream(&brokenDependenciesFile);
    foreach(int branch, parentPointToSelf){
        brokenDependenciesFileStream << branch << "\n";
    }
    brokenDependenciesFileStream.flush();
    brokenDependenciesFile.close();


    //Read the data from the null models
    std::vector<double> nullModelData = readNullModels("/random_neat_mut_data.dat");

    //Write correlation files
    writeBranchingVsMetric(directory, "/modularity_branching.dat", modularityMap, reverseFamilyMap);
    writeBranchingVsMetric(directory, "/forward_hierarchy_branching.dat", forwardHierarchyMap, reverseFamilyMap);
    writeBranchingVsMetric(directory, "/backward_hierarchy_branching.dat", backwardHierarchyMap, reverseFamilyMap);
    writeBranchingVsMetric(directory, "/corrected_modularity_branching.dat", modularityMap, reverseFamilyMap, nullModelData[0]);
    writeBranchingVsMetric(directory, "/corrected_forward_hierarchy_branching.dat", forwardHierarchyMap, reverseFamilyMap, nullModelData[1]);
    writeBranchingVsMetric(directory, "/corrected_backward_hierarchy_branching.dat", backwardHierarchyMap, reverseFamilyMap, nullModelData[2]);

    //Write the information from the modularity map to a file
//    QFile modularity_file(_genomeDir + "/modularity_branching.dat");
//    modularity_file.open(QIODevice::WriteOnly | QIODevice::Text);
//    QTextStream modularity_file_out(&modularity_file);
//    for (uint i = 0; i < directory.count(); ++i){
//        int currentBranch = util::toInt(directory[i].toStdString());
//        if(modularityMap.count(currentBranch)){
//            if(reverseFamilyMap.count(currentBranch)){
//                modularity_file_out << reverseFamilyMap[currentBranch].size() << " " << modularityMap[currentBranch] << "\n";
//            } else {
//                modularity_file_out << 0 << " " << modularityMap[currentBranch] << "\n";
//            }
//        }
//    }
//    modularity_file.close();

    //Variables to keep track of time
    time_t currentTime = 0;
    time_t previousTime = 0;
    double processed = 0;
    double expected = 0;
    double expectedStable = 0;
    double stability_threshold = 0.2;
    size_t stableTime = 0;
    bool stable = false;


    //Open the genome file
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream out(&file);

    //Open the random null model file
    QFile randomNullModelFile(globalSettings->getGenomeDir() + "/random_null_data.dat");
    randomNullModelFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream randomNullModelFileStream(&randomNullModelFile);

    QFile randomNeatMutNullModelFile(globalSettings->getGenomeDir() + "/random_neat_mut_data.dat");
    randomNeatMutNullModelFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream randomNeatMutNullModelFileStream(&randomNeatMutNullModelFile);

    QFile randomFfIoNullModelFile(globalSettings->getGenomeDir() + "/random_ff_io_data.dat");
    randomFfIoNullModelFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream randomFfIoNullModelFileStream(&randomFfIoNullModelFile);

    QFile randomFfNullModelFile(globalSettings->getGenomeDir() + "/random_ff_data.dat");
    randomFfNullModelFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream randomFfNullModelFileStream(&randomFfNullModelFile);

    //For each network
    uint directoryCount = directory.count();
    for (uint i = 0; i < directoryCount; ++i){
        int currentBranch = util::toInt(directory[i].toStdString());
        std::cout << "Processing individual: " << i << "  branch: " << currentBranch << "... ";
        if(familyMap.count(currentBranch)){
            std::cout << " Branch already in map, break." << std::endl;
            continue;
        }

        //Time current operation
        std::time(&currentTime);
        if(previousTime){
            double seconds = std::difftime(currentTime,previousTime);
            expected *= (processed/(processed+1.0));
            expected += (seconds)/(processed+1.0);
            processed += 1.0;

            if(std::abs(expected - expectedStable) > stability_threshold){
                expectedStable = expected;
                stableTime = 0;
            } else {
                stableTime++;
                stable = true;
            }

            int secondsRemaining = expectedStable * (directoryCount-i);
            int minutes = secondsRemaining/60;
            int hours = minutes/60;
            secondsRemaining = secondsRemaining % 60;
            minutes = minutes % 60;

            std::cout << " Processing time (seconds): " << seconds << " average: " << expected;
            std::cout << " remaining: ";
            if(stable){
                std::cout << hours << ":" << minutes << ":" << secondsRemaining;
            } else {
                std::cout << "calculating";
            }
        }
        previousTime = currentTime;
        std::cout << std::endl;

        try{
            _loadBranch(currentBranch);
        } catch(CeParseException& e){
            std::cout << "##################################################" << std::endl;
            std::cout << e.what() << std::endl;
            std::cout << "##################################################" << std::endl;
            continue;
        }

        //Create a thumbnail image
        if(image){
            QImage nodeViewImage = nodeviewWidget->getNodeviews().first()->getImage()->copy(0,0,256,256);
            QString name = globalSettings->getGenomeDir() + "/" + directory[i] + "/thumb.png";
            nodeViewImage.save(name);
        }

        //Calculate hierarchy
        Cppn* originalCppn = cppnWidget->getCppn();
        double hierarchyForwardOriginal =  getGlobalReachingCentrality(*originalCppn, forwardReachableNodeCount());
        double hierarchyBackwardOriginal =  getGlobalReachingCentrality(*originalCppn, backwardReachableNodeCount());

        dbg::out(dbg::info, "analysis") << "Original network:" << std::endl;
        dbg::out(dbg::info, "analysis") << "  Modularity: " << _qscore <<
                " hierarchy forward: " << hierarchyForwardOriginal <<
                " hierarchy backward: " << hierarchyBackwardOriginal <<
                " nr of nodes: " << originalCppn->getNrOfNodes() <<
                " nr of edges: " << originalCppn->getNrOfEdges() <<
                std::endl;

        bool color = originalCppn->getOutputs().size() > 1;

        //Generate a set of random networks
        dbg::out(dbg::info, "analysis") << "Generating random networks: " << std::endl;

        for(size_t j=0; j<10; ++j){
//            generateRandomNetwork(currentBranch, j, randomNullModelFileStream);
            dbg::out(dbg::info, "analysis") << "  Generating random NEAT network" << std::endl;
            Cppn* randomNeatMut = _createRandomCppnNeatMutations(originalCppn->getNrOfNodes(), originalCppn->getNrOfEdges(), color);
            dbg::out(dbg::info, "analysis") << "  Generating random FF IO network" << std::endl;
            Cppn* randomFfIoAddCon =  _createRandomCppnFfIoAddCon(originalCppn->getNrOfNodes(), originalCppn->getNrOfEdges(), color);
            dbg::out(dbg::info, "analysis") << "  Generating random FF network" << std::endl;
            Cppn* randomFfAddCon = _createRandomCppnFfAddCon(originalCppn->getNrOfNodes(), originalCppn->getNrOfEdges());

            dbg::out(dbg::info, "analysis") << "  Dumping random NEAT network" << std::endl;
            _dumpCppn(randomNeatMut, currentBranch, j, randomNeatMutNullModelFileStream);
            dbg::out(dbg::info, "analysis") << "  Dumping random FF IO network" << std::endl;
            _dumpCppn(randomFfIoAddCon, currentBranch, j, randomFfIoNullModelFileStream);
            dbg::out(dbg::info, "analysis") << "  Dumping random FF network" << std::endl;
            _dumpCppn(randomFfAddCon, currentBranch, j, randomFfNullModelFileStream);

            delete randomNeatMut;
            delete randomFfIoAddCon;
            delete randomFfAddCon;
        }
        randomNullModelFileStream.flush();
        randomNeatMutNullModelFileStream.flush();
        randomFfIoNullModelFileStream.flush();
        randomFfNullModelFileStream.flush();

        //Write all relevant information to a file
        out << currentBranch <<
                " " << _parent <<
                " " << _qscore <<
                " " << hierarchyForwardOriginal <<
                " " << hierarchyBackwardOriginal <<
                " " << _maxGeneration <<"\n";
        out.flush();

        //Delete cppn explicitly (easier to debug memory leaks).
        cppnWidget->deleteCppn();
    }

    file.close();
}

void Window::generateRandomNetwork(int branch, size_t iteration, QTextStream& stream){
    dbg::trace trace("analysis", DBG_HERE);
    Cppn cppn;
    Cppn* originalCppn = cppnWidget->getCppn();
    cppn.copy(originalCppn);
    size_t rewires = 10*cppn.getNrOfEdges();
    size_t tries = 200*cppn.getNrOfEdges();
    dbg::out(dbg::info, "analysis") << "Generating random network" << std::endl;
    while(rewires > 0 && tries > 0){
        --tries;
        size_t connectionIndex1 = randIndex(cppn.getNrOfEdges());
        size_t connectionIndex2 = randIndex(cppn.getNrOfEdges());
        Edge* edge1 = cppn.getEdge(connectionIndex1);
        Edge* edge2 = cppn.getEdge(connectionIndex2);
        Node* source1 = edge1->sourceNode();
        Node* target1 = edge1->destNode();
        Node* source2 = edge2->sourceNode();
        Node* target2 = edge2->destNode();
        if(cppn.connected(source1, target2) || cppn.connected(source2, target1)){
            continue;
        }
        cppn.rewireConnection(edge1, source1, target2);
        cppn.rewireConnection(edge2, source2, target1);
        if(cppn.connectionCausesCycle(edge1) || cppn.connectionCausesCycle(edge2)){
            cppn.rewireConnection(edge1, source1, target1);
            cppn.rewireConnection(edge2, source2, target2);
            continue;
        }
        --rewires;
    }
    dbg::out(dbg::info, "analysis") << "Generating random network done" << std::endl;

    size_t possibleConnections = 0;
    size_t matchingConnections = 0;
    size_t actualConnections = cppn.getNrOfEdges();
    for(size_t i=0; i<cppn.getNrOfNodes(); ++i){
        for(size_t j=0; j<cppn.getNrOfNodes(); ++j){
            if(cppn.getNode(i)->outgoingEdges().size() > 0 &&
                    cppn.getNode(j)->incomingEdges().size() > 0){
                ++possibleConnections;
                if(cppn.connected(i,j) && originalCppn->connected(i,j)){
                    ++matchingConnections;
                }
            }
        }
    }
    double chanceOfSelection = double(actualConnections)/double(possibleConnections);
    double expectedValue = chanceOfSelection*double(actualConnections);

    double modularity = cppn.calculateModularity();
    double hierarchyForwardRandom =  getGlobalReachingCentrality(cppn, forwardReachableNodeCount());
    double hierarchyBackwardRandom =  getGlobalReachingCentrality(cppn, backwardReachableNodeCount());

    stream << branch <<
            " " << iteration <<
            " " << modularity <<
            " " << hierarchyForwardRandom <<
            " " << hierarchyBackwardRandom <<
            " " << rewires <<
            " " << tries <<
            " " << possibleConnections <<
            " " << actualConnections <<
            " " << matchingConnections <<
            " " << expectedValue << "\n";

    dbg::out(dbg::info, "analysis") <<
            "  Modularity: "<< modularity <<
            " hierarchy forward: " << hierarchyForwardRandom <<
            " hierarchy backward: " << hierarchyBackwardRandom <<
            " rewires: " << rewires <<
            " retries: " << tries <<
            " possible connections: " << possibleConnections <<
            " actual connections: " << actualConnections <<
            " matching connections: " << matchingConnections <<
            " expected value: " << expectedValue <<
            std::endl;
}


void Window::addConMut(){
    dbg::trace trace("window", DBG_HERE);
    Edge* edge = _createConMutEdge(cppnWidget->getCppn());
    if(edge) cppnWidget->addEdge(edge);
}

void Window::addNodeMut(){
    dbg::trace trace("window", DBG_HERE);
    QPair<QList<Edge*>, Node*> nodeAndEdges = _createNodeMutNodeAndEdges(cppnWidget->getCppn());
    cppnWidget->removeEdge(nodeAndEdges.first[0]);
    cppnWidget->addNode(nodeAndEdges.second);
    cppnWidget->addEdge(nodeAndEdges.first[1]);
    cppnWidget->addEdge(nodeAndEdges.first[2]);
}

void Window::_dumpCppn(Cppn* cppn, int branch, size_t iteration, QTextStream& stream){
    dbg::trace trace("analysis", DBG_HERE);
    Cppn* originalCppn = cppnWidget->getCppn();
    size_t possibleConnections = 0;
    size_t matchingConnections = 0;
    size_t actualConnections = cppn->getNrOfEdges();
    for(size_t i=0; i<cppn->getNrOfNodes(); ++i){
        for(size_t j=0; j<cppn->getNrOfNodes(); ++j){
            if(cppn->getNode(i)->outgoingEdges().size() > 0 &&
                    cppn->getNode(j)->incomingEdges().size() > 0){
                ++possibleConnections;
                if(cppn->connected(i,j) && originalCppn->connected(i,j)){
                    ++matchingConnections;
                }
            }
        }
    }
    double chanceOfSelection = double(actualConnections)/double(possibleConnections);
    double expectedValue = chanceOfSelection*double(actualConnections);

    double modularity = cppn->calculateModularity();
    double hierarchyForwardRandom =  getGlobalReachingCentrality(*cppn, forwardReachableNodeCount());
    double hierarchyBackwardRandom =  getGlobalReachingCentrality(*cppn, backwardReachableNodeCount());

    stream << branch <<
            " " << iteration <<
            " " << modularity <<
            " " << hierarchyForwardRandom <<
            " " << hierarchyBackwardRandom <<
            " " << possibleConnections <<
            " " << actualConnections <<
            " " << matchingConnections <<
            " " << expectedValue << "\n";

    dbg::out(dbg::info, "analysis") <<
            "  Modularity: "<< modularity <<
            " hierarchy forward: " << hierarchyForwardRandom <<
            " hierarchy backward: " << hierarchyBackwardRandom <<
            " nb of edges: " << actualConnections <<
            " nb of nodes: " << cppn->getNrOfNodes() <<
            " possible connections: " << possibleConnections <<
            " matching connections: " << matchingConnections <<
            " expected value: " << expectedValue <<
            std::endl;
}

/**
 * Generates a random cppn network according to NEAT mutation operators.
 *
 * Abides by the following constraints:
 * - The resulting network has a specific number of nodes and connections
 * - The network is feed-forward
 * - Inputs never have incoming connections, outputs never have outgoing connections
 * - There exists a path from every input to every output
 * - Every node is reachable from at least one input,
 *   and every node can reach at least one output
 */
Cppn* Window::_createRandomCppnNeatMutations(size_t nrOfNeurons, size_t nrOfConnections, bool color){
    dbg::trace trace("analysis", DBG_HERE);
    QList<MutationType> mutations;

    //Create a fully connected network
    Cppn* result = _createInitialCppn(color);

   //Apply the requested mutations in a random order
   dbg::assertion(DBG_ASSERTION(result->getNrOfNodes() <= nrOfNeurons));
   for(size_t i=result->getNrOfNodes(); i<nrOfNeurons; ++i){
       mutations.push_back(addNeuron);
       --nrOfConnections;
   }

   dbg::assertion(DBG_ASSERTION(result->getNrOfEdges() <= nrOfConnections));
   for(size_t i=result->getNrOfEdges(); i<nrOfConnections; ++i){
       mutations.push_back(addConnection);
   }

   if(mutations.size() > 1){
       //Always start with an add neuron mutation, if possible
       randomShuffle(mutations.begin()+1, mutations.end());
   }

   QPair<QList<Edge*>, Node*> nodeAndEdges;
   Edge* edge;

   while(!mutations.empty()){
       MutationType mutationType = mutations.front();

       switch(mutationType){
       case addNeuron:
           dbg::out(dbg::info, "mutation") << "Adding neuron" << std::endl;
           nodeAndEdges = _createNodeMutNodeAndEdges(result);
           result->removeConnection(nodeAndEdges.first[0]);
           result->addNode(nodeAndEdges.second);
           result->addConnection(nodeAndEdges.first[1]);
           result->addConnection(nodeAndEdges.first[2]);
           mutations.pop_front();
           break;
       case addConnection:
           dbg::out(dbg::info, "mutation") << "Attempt to add edge" << std::endl;
           edge = _createConMutEdge(result);
           if(edge){
               dbg::out(dbg::info, "mutation") << "Adding edge success" << std::endl;
               result->addConnection(edge);
               mutations.pop_front();
           } else {
               dbg::out(dbg::info, "mutation") << "Adding edge failed" << std::endl;
               int index = mutations.indexOf(addNeuron);
               if(index != -1){
                   dbg::out(dbg::info, "mutation") << "Trying add neuron mutation" << std::endl;
                   mutations.swap(0, index);
               } else {
                   std::cout << "Warning: CPPN does not have the requested size" << std::endl;
                   //We have failed to produce a network of the required size
                   //Return our current result anyway.
                   return result;
               }
           }
           break;
       default:
           dbg::sentinel(DBG_HERE);
       }
   }
   return result;
}

/**
 * Creates a random feed-forward cppn with inputs and outputs by first
 * adding the inputs and outputs, then all hidden neurons, and then adding all connections.
 *
 * Abides by the following constraints:
 * - The resulting network has a specific number of nodes and connections
 * - The network is feed-forward
 * - Inputs never have incoming connections, outputs never have outgoing connections
 */
Cppn* Window::_createRandomCppnFfIoAddCon(size_t nrOfNeurons, size_t nrOfConnections, bool color){
    dbg::trace trace("analysis", DBG_HERE);
    Cppn* result = _createCppnWithIO(color);
    _addRandomNodes(result, nrOfNeurons - result->getNrOfNodes());
    _addRandomConnections(result, nrOfConnections);
   return result;
}

/**
 * Creates a random feed-forward cppn by first adding all neurons, then adding all connections.
 *
 * Does NOT generate a valid cppn, the network does not have any inputs or outputs.
 *
 * Abides by the following constraints:
 * - The resulting network has a specific number of nodes and connections
 * - The network is feed-forward
 */
Cppn* Window::_createRandomCppnFfAddCon(size_t nrOfNeurons, size_t nrOfConnections){
    dbg::trace trace("analysis", DBG_HERE);
    Cppn* result = new Cppn();

    _addRandomNodes(result, nrOfNeurons);
    _addRandomConnections(result, nrOfConnections);
   return result;
}

/**
 * Adds random nodes to the supplied network
 *
 * Does not connect the nodes to anything.
 */
Cppn* Window::_addRandomNodes(Cppn* cppn, size_t nrOfNodes){
    dbg::trace trace("analysis", DBG_HERE);
   for(size_t i=0; i<nrOfNodes; ++i){
       cppn->addNode(new Node("0", cppn->getNextId(), XML_TYPE_HIDDEN, util::getRandomActivationFunction()));
   }
   return cppn;
}

/**
 * Adds random connections to the supplied network, keeping the network feed-forward.
 */
Cppn* Window::_addRandomConnections(Cppn* cppn, size_t nrOfConnections){
    dbg::trace trace("analysis", DBG_HERE);
    for(size_t i=0; i<nrOfConnections; ++i){
        Edge* edge = _createConMutEdge(cppn);
        if(!edge) return cppn;
        cppn->addConnection(edge);
    }
   return cppn;
}

/**
 * Creates an initial cppn network with 4 inputs, 1 (grey) or 3 (color) outputs,
 * where all inputs are connected to all outputs.
 */
Cppn* Window::_createInitialCppn(bool color){
    dbg::trace trace("analysis", DBG_HERE);
    std::string branch = "0";
    std::string id;
    qreal weight;

    //Create a fully connected network
    Cppn* result = _createCppnWithIO(color);

    foreach(Node* input, result->getInputs()){
        foreach(Node* output, result->getOutputs()){
            weight = randDouble(-3.0, 3.0);
            result->addConnection(new Edge(branch, result->getNextId(), input, output, weight, weight));
        }
    }

    return result;
}

/**
 * Creates a cppn that only has the basic inputs and outputs.
 */
Cppn* Window::_createCppnWithIO(bool color){
    dbg::trace trace("analysis", DBG_HERE);
    std::string branch = "0";

    //Create a fully connected network
    Cppn* result = new Cppn();
    result->addNode(new Node(branch, result->getNextId(), XML_TYPE_INPUT, XML_LINEAR, INPUT_X));
    result->addNode(new Node(branch, result->getNextId(), XML_TYPE_INPUT, XML_LINEAR, INPUT_Y));
    result->addNode(new Node(branch, result->getNextId(), XML_TYPE_INPUT, XML_LINEAR, INPUT_D));
    result->addNode(new Node(branch, result->getNextId(), XML_TYPE_INPUT, XML_LINEAR, INPUT_BIAS));

    if(color){
        result->addNode(new Node(branch, result->getNextId(), XML_TYPE_OUTPUT, util::getRandomActivationFunction(), OUTPUT_BRIGTHNESS));
        result->addNode(new Node(branch, result->getNextId(), XML_TYPE_OUTPUT, util::getRandomActivationFunction(), OUTPUT_SATURATION));
        result->addNode(new Node(branch, result->getNextId(), XML_TYPE_OUTPUT, util::getRandomActivationFunction(), OUTPUT_HUE));
    } else {
        result->addNode(new Node(branch, result->getNextId(), XML_TYPE_OUTPUT, XML_LINEAR, OUTPUT_INK));
    }

    return result;
}

/**
 * Creates a new edge that is randomly connected between two unconnected nodes.
 */
Edge* Window::_createConMutEdge(Cppn* cppn){
    dbg::trace trace("mutation", DBG_HERE);
    QSet<Node*> illegalTargetNodes;
    QList<Node*> legalSourceNodes;
    QList<Node*> legalTargetNodes;
    //typedef QList<Node*> node_list_t;

    dbg::out(dbg::info, "mutation") << "cppn address: " << cppn << std::endl;

    //Determine the source node
    foreach(Node* node, cppn->getNodes()){
        dbg::check_ptr(node, DBG_HERE);
        if(node->getNodeType() != Node::output){
            legalSourceNodes.push_back(node);
        }

//        foreach(Node* target, cppn->getNodes()){
//            dbg::check_ptr(target, DBG_HERE);
//            if(!cppn->connected(node, target) &&
//                    node!=target &&
//                    node->getNodeType() != Node::output &&
//                    target->getNodeType() != Node::input){
//                std::cout << node->getId() << " is not connected to " << target->getId()  << std::endl;
//            }
//        }
    }

    while(!legalSourceNodes.empty()){
        dbg::out(dbg::info, "mutation") << "Number of legal sources: " << legalSourceNodes.size() << std::endl;
//        size_t sourceNodeIndex = randIndex(legalSourceNodes.size());
        Node* sourceNode = randElement(legalSourceNodes);
        dbg::out(dbg::info, "mutation") <<
                "Source chosen: " << sourceNode->getId() <<
                " type: " << sourceNode->getType() << std::endl;

        //Determine the legal targets
        illegalTargetNodes = cppn->getInputs().toSet();
        illegalTargetNodes.unite(cppn->getPredecessors(sourceNode));
        foreach(Node* node, cppn->getNodes()){
            if(!illegalTargetNodes.contains(node) && !cppn->connected(sourceNode, node)){
                legalTargetNodes.push_back(node);
            }
        }
        dbg::out(dbg::info, "mutation") << "Number of legal targets: " << legalTargetNodes.size() << std::endl;

        if(legalTargetNodes.empty()){
            legalSourceNodes.removeAll(sourceNode);
        } else {
            Node* targetNode = randElement(legalTargetNodes);
            dbg::out(dbg::info, "mutation") << "Target chosen: " << targetNode->getId() << " type: " << targetNode->getType() << std::endl;
            qreal weight = randDouble(-3.0, 3.0);
            Edge* edge = new Edge("0", cppn->getNextId(), sourceNode, targetNode, weight, weight);
            return edge;
        }
    }
    return 0;
}

/**
 * Creates a node and two connections that are supposed to be splices onto an existing connection.
 *
 * Returns a pair containing a list of three Edges and a Node.
 * The first edge is the selected edge, and this edge should be removed from the cppn.
 * The second and third edges are the new edges replacing the old edge, and they are supposed
 * to be added to the cppn after the node has been added.
 * The node should simply be added the the cppn.
 */
QPair<QList<Edge*>, Node*> Window::_createNodeMutNodeAndEdges(Cppn* cppn){
    dbg::trace trace("mutation", DBG_HERE);
    QPair<QList<Edge*>, Node*> result;
    Node* node;

    qreal weight;

    //Select random edge
    size_t selectedEdgeIndex = randIndex(cppn->getNrOfEdges());
    Edge* selectedEdge = cppn->getEdge(selectedEdgeIndex);

    std::string branch = fileInformation->branch;
    std::string id = util::toString(cppn->getMaxId()+1);
    std::string type = XML_TYPE_HIDDEN;
    std::string activationFunction = util::getRandomActivationFunction();

    node = new Node(branch, id, type, activationFunction);

    weight = randDouble(-3.0, 3.0);
    id = util::toString(cppn->getMaxId()+2);
    Edge* edge1 = new Edge(branch, id, selectedEdge->sourceNode(), node, weight, weight);

    weight = randDouble(-3.0, 3.0);
    id = util::toString(cppn->getMaxId()+3);
    Edge* edge2 = new Edge(branch, id, node, selectedEdge->destNode(), 1.0, 1.0);

    //Create the return object
    result.first.push_back(selectedEdge);
    result.first.push_back(edge1);
    result.first.push_back(edge2);
    result.second = node;
    return result;
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

//Scan and screen capture
void Window::startFilm(){
    dbg::trace trace("window", DBG_HERE);
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
#endif //USE_FFMPEG

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
        dbg::out(dbg::info, "window") << "exportToImageSeries: no edge selected, return;" << std::endl;
        return;
    }

    QString newFileName = util::getSupportedFilename(this, tr("Chose save directory"));
    if(newFileName.isEmpty()){
        dbg::out(dbg::info, "window") << "exportToImageSeries: file name is empty, return;" << std::endl;
        return;
    }

    QString baseName = util::getBase(newFileName);
    QString folderStartName = util::getNameFromBase(baseName);
    QString edgeName = QString::fromStdString(edge->getId()) + "_" + QString::fromStdString(edge->getBranch());
    QString directory = baseName + "_" + edgeName;

    extention = util::getExtention(newFileName);
    QDir().mkdir(directory);

    double originalWeight = weightWidget->getValue();
    double start = weightWidget->getScanStartValue();
    double stop = weightWidget->getScanStopValue();
    double step = weightWidget->getStepSizeValue();
    int imageNumber = 1;

    dbg::out(dbg::info, "window") << "Start: " << start << " stop: " << stop << " step: " << step << std::endl;
    for(double weight=start; weight<=stop; weight+=step){
        dbg::out(dbg::info, "window") << "Creating image for weight: " << weight << std::endl;
        cppnWidget->setWeight(weight);
        QImage nodeViewImage = nodeviewWidget->getFinalNodeView()->getImage()->copy(0,0,256,256);
        QString imageNbStr = QString::number(imageNumber);
        QString weightStr = _formatWeight(weight);
        QString name = directory + folderStartName + "_" + imageNbStr + "_" + edgeName + "_" + weightStr + extention;
        dbg::out(dbg::info, "window") << "Saving image as: " << name.toStdString() << std::endl;

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

//Update functions
void Window::updateEdge(Edge* edge){
    dbg::trace trace("window", DBG_HERE);
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
    dbg::trace trace("window", DBG_HERE);
//	std::cout << edge << std::endl;
	if(node){
		weightWidget->edgeSelected(false);
		weightWidget->setNode(util::toQString(node->getBranch()  + "_" + node->getId()));
	} else {

	}
}

void Window::executeCommand(ComBase* command){
    dbg::trace trace("window", DBG_HERE);
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
    dbg::trace trace("window", DBG_HERE);
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
    dbg::trace trace("window", DBG_HERE);
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
		DragAndDropGraphicsView* dargAndDropGraphicsView = qobject_cast<DragAndDropGraphicsView*> (widget);
		if(dargAndDropGraphicsView){
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
    dbg::trace trace("window", DBG_HERE);
	_qscore = qscore;
	modularityLabel->setText(modularityText + util::toQString(_qscore) + QString(" ") + generationText + util::toQString(_currentGeneration));
}

void Window::_updateCurrentGeneration(int generation){
    dbg::trace trace("window", DBG_HERE);
	_currentGeneration = generation;
	modularityLabel->setText(modularityText + util::toQString(_qscore) + QString(" ") + generationText + util::toQString(_currentGeneration));
}

//Create action
QAction* Window::_createAction(QString name, QString statusBarTip, const char *member){
    dbg::trace trace("window", DBG_HERE);
    QAction* action = new QAction(name, this);
    action->setStatusTip(statusBarTip);
    connect(action, SIGNAL(triggered()), this, member);
    addAction(action);
    return action;
}

void Window::preferences(){
    PreferencesWidget* preferences = new PreferencesWidget();
    preferences->show();
    preferences->exec();
    delete preferences;
}
