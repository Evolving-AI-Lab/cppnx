//Standard includes
#include <cmath>

//QT includes
#include <QtGui>

//Local includes
#include "CE_CppnWidget.h"
#include "CE_Util.h"
#include "CE_CppnParser.h"
#include "CE_Cppn.h"
#include "CE_CppnWriter.h"
#include "CX_ComSetBookends.h"
#include "CX_Debug.hpp"

//const double CppnWidget::left_border_min = 0.0;
//const double CppnWidget::right_border_min = 0.0;
//const double CppnWidget::top_border_min = 0.0;
//const double CppnWidget::bottom_border_min = 0.0;
//const double CppnWidget::left_border_min = -400.0;
//const double CppnWidget::right_border_min = 400.0;
//const double CppnWidget::top_border_min = -600.0;
//const double CppnWidget::bottom_border_min = 600.0;
const double CppnWidget::extra_space = 1;
const double CppnWidget::snapshot_super_resolution = 2;
const qreal CppnWidget::minimum_zoom_factor = 0.07;
const qreal CppnWidget::maximum_zoom_factor = 100.0;


bool xPosLessThan(Node* node1, Node* node2) {
    return node1->pos().x() < node2->pos().x();
}

bool yPosLessThan(Node* node1, Node* node2) {
    return node1->pos().y() < node2->pos().y();
}



CppnWidget::CppnWidget(QWidget* widget)
    : ContextMenuGraphicsView(widget),
      left_sorted(MovableObject::left),
      right_sorted(MovableObject::right),
      top_sorted(MovableObject::top),
      bottom_sorted(MovableObject::bottom)

{
    dbg::trace trace("cppnwidget", DBG_HERE);

//    _current_favourite_index = 0;
	cppn = 0;
	blockWeightUpdates = false;
	firstClick = false;
	_firstNodeSelected = 0;
	labelMode = Edge::showLabel;
	lineMode = Edge::curved;
    _signMode = Edge::showSignIfNoLabel;
    _lineWidthMode = Edge::weightWidth;
    _annotationMode = Edge::showAnnotation;
	nodeLabelMode = Node::labels;
	_legend = 0;

//    left_border = left_border_min;
//    right_border = right_border_min;
//    top_border = top_border_min;
//    bottom_border = bottom_border_min;

//    QGraphicsScene *scene = new QGraphicsScene(this);
    scene()->setSceneRect(left_border, top_border, (-left_border)+right_border, (-top_border)+bottom_border);
    connect(scene(), SIGNAL(selectionChanged()), this, SLOT(updateSelection()));
//    setScene(scene);


    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setDragMode(ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    // Add to side bar action
    addNodeviewAction = new QAction(tr("&Add to sidebar"), this);
    addNodeviewAction->setShortcut(tr("Alt+A"));
    addNodeviewAction->setStatusTip(tr("Add the selected node to the sidebar"));
    connect(addNodeviewAction, SIGNAL(triggered()), this, SLOT(addNodeView()));
    addAction(addNodeviewAction);
    _nodeActions.append(addNodeviewAction);


    // Node view actions
    nodeLabelAction = new QAction(tr("Show node labels"), this);
    nodeLabelAction->setShortcut(tr("Shift+1"));
    nodeLabelAction->setStatusTip(tr("Shows the label of each node"));
    nodeLabelAction->setCheckable(true);
    connect(nodeLabelAction,SIGNAL(triggered()), this, SLOT(setNodeLabelView()));
    addAction(nodeLabelAction);

    nodeModuleAction = new QAction(tr("Show node modules"), this);
    nodeModuleAction->setShortcut(tr("Shift+2"));
    nodeModuleAction->setStatusTip(tr("Shows the module of each node"));
    nodeModuleAction->setCheckable(true);
    connect(nodeModuleAction,SIGNAL(triggered()), this, SLOT(setNodeModuleView()));
    addAction(nodeModuleAction);

    nodeLabelNoImageAction = new QAction(tr("Show labels without image"), this);
    nodeLabelNoImageAction->setShortcut(tr("Shift+3"));
    nodeLabelNoImageAction->setStatusTip(tr("Shows the label of each node without showing the image"));
    nodeLabelNoImageAction->setCheckable(true);
    connect(nodeLabelNoImageAction,SIGNAL(triggered()), this, SLOT(setNodeLabelNoImageView()));
    addAction(nodeLabelNoImageAction);

    nodeViewGroup = new QActionGroup(this);
    nodeViewGroup->addAction(nodeLabelAction);
    nodeViewGroup->addAction(nodeModuleAction);
    nodeViewGroup->addAction(nodeLabelNoImageAction);
    nodeLabelAction->setChecked(true);


    // Zoom actions
    _zoomInAction = new QAction(tr("Zoom In"), this);
    _zoomInAction->setShortcut(QKeySequence::ZoomIn);
    _zoomInAction->setStatusTip(tr("Increase the size of the representation of the network"));
    connect(_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
    addAction(_zoomInAction);

    _zoomOutAction = new QAction(tr("Zoom Out"), this);
    _zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    _zoomOutAction->setStatusTip(tr("Decrease the size of the representation of the network"));
    connect(_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
    addAction(_zoomOutAction);

    _setZoomAction = new QAction(tr("Set zoom..."), this);
    _setZoomAction->setShortcut(tr("Alt+z"));
    _setZoomAction->setStatusTip(tr("Decrease the size of the representation of the network"));
    connect(_setZoomAction, SIGNAL(triggered()), this, SLOT(setZoom()));
    addAction(_setZoomAction);


    //Create edge label actions
    _showLabelAction = new QAction(tr("Show edge label"), this);
    _showLabelAction->setShortcut(tr("Ctrl+1"));
    _showLabelAction->setStatusTip(tr("Show label color on all nodes and edges"));
    _showLabelAction->setCheckable(true);
    connect(_showLabelAction, SIGNAL(triggered()), this, SLOT(setEdgeShowLabel()));
    addAction(_showLabelAction);
    _showLabelAction->setChecked(true);

    _noLabelAction = new QAction(tr("Hide edge label"), this);
    _noLabelAction->setShortcut(tr("Ctrl+2"));
    _noLabelAction->setStatusTip(tr("Hide label color on all nodes and edges"));
    _noLabelAction->setCheckable(true);
    connect(_noLabelAction, SIGNAL(triggered()), this, SLOT(setEdgeNoLabel()));
    addAction(_noLabelAction);

    _edgeShowLabelGroup = new QActionGroup(this);
    _edgeShowLabelGroup->addAction(_showLabelAction);
    _edgeShowLabelGroup->addAction(_noLabelAction);


    //Create edge sign actions
    _showSignAction = new QAction(tr("Show edge sign"), this);
    _showSignAction->setShortcut(tr("Ctrl+3"));
    _showSignAction->setStatusTip(tr("Show the sign of the connection (positive as green, negative as red) on all connections"));
    _showSignAction->setCheckable(true);
    connect(_showSignAction, SIGNAL(triggered()), this, SLOT(setEdgeShowSign()));
    addAction(_showSignAction);

    _showSignIfNoLabelAction = new QAction(tr("Show edge sign if unlabeled"), this);
    _showSignIfNoLabelAction->setShortcut(tr("Ctrl+4"));
    _showSignIfNoLabelAction->setStatusTip(tr("Show the sign of a connection only if the connection is unlabeled"));
    _showSignIfNoLabelAction->setCheckable(true);
    _showSignIfNoLabelAction->setChecked(true);
    connect(_showSignIfNoLabelAction, SIGNAL(triggered()), this, SLOT(setEdgeShowSignIfNoLabel()));
    addAction(_showSignIfNoLabelAction);

    _noSignAction = new QAction(tr("Hide edge sign"), this);
    _noSignAction->setShortcut(tr("Ctrl+5"));
    _noSignAction->setStatusTip(tr("Do not show the sign on any of the connections"));
    _noSignAction->setCheckable(true);
    connect(_noSignAction, SIGNAL(triggered()), this, SLOT(setEdgeNoSign()));
    addAction(_noSignAction);

    _edgeShowSignGroup = new QActionGroup(this);
    _edgeShowSignGroup->addAction(_showSignAction);
    _edgeShowSignGroup->addAction(_noSignAction);
    _edgeShowSignGroup->addAction(_showSignIfNoLabelAction);


    //Create edge width actions
    _weightEdgeWidthAction = new QAction(tr("Weight dependent edge width"), this);
    _weightEdgeWidthAction->setShortcut(tr("Ctrl+8"));
    _weightEdgeWidthAction->setStatusTip(tr("The width of all edges will depend on their weight, were stronger edges will be wider"));
    _weightEdgeWidthAction->setCheckable(true);
    _weightEdgeWidthAction->setChecked(true);
    connect(_weightEdgeWidthAction, SIGNAL(triggered()), this, SLOT(setEdgeWeightWidth()));
    addAction(_weightEdgeWidthAction);


    _fixedEdgeWidthAction = new QAction(tr("Fixed edge width"), this);
    _fixedEdgeWidthAction->setShortcut(tr("Ctrl+9"));
    _fixedEdgeWidthAction->setStatusTip(tr("The width of all edges will be the same, regardless of their weight"));
    _fixedEdgeWidthAction->setCheckable(true);
    connect(_fixedEdgeWidthAction, SIGNAL(triggered()), this, SLOT(setEdgeFixedWidth()));
    addAction(_fixedEdgeWidthAction);


    // Create action group
    _edgeWidthGroup = new QActionGroup(this);
    _edgeWidthGroup->addAction(_weightEdgeWidthAction);
    _edgeWidthGroup->addAction(_fixedEdgeWidthAction);

    //Create find unlabeled edge action
    _findUnlabeledEdgeAction = new QAction(tr("Unlabeled edge"), this);
    _findUnlabeledEdgeAction->setShortcut(tr("Alt+a"));
    _findUnlabeledEdgeAction->setStatusTip(tr("Selects an unlabeled edge, if there are any"));
    connect(_findUnlabeledEdgeAction, SIGNAL(triggered()), this, SLOT(findUnlabeledEdge()));
    addAction(_findUnlabeledEdgeAction);


    _findEdgeByIdAction = createAction(tr("Find edge by ID"),
    		tr("Finds an edge by its ID"),
			SLOT(findEdgeById()));
    _findEdgeByIdAction->setShortcut(QKeySequence::Find);

    // Activation function actions
    _setSin = createAction(tr("Sine"),
    		tr("Set the sine activation function"),
			SLOT(setSin()));
    _setSin->setShortcut(tr("Ctrl+Shift+n"));
    _nodeActions.append(_setSin);
    _setCos = createAction(tr("Cosine"),
    		tr("Set the cosine activation function"),
			SLOT(setCos()));
    _setCos->setShortcut(tr("Ctrl+Shift+c"));
    _nodeActions.append(_setCos);
    _setGaus = createAction(tr("Gaussian"),
    		tr("Set the Guassian activation function"),
			SLOT(setGaus()));
    _setGaus->setShortcut(tr("Ctrl+Shift+g"));
    _nodeActions.append(_setGaus);
    _setSigmoid = createAction(tr("Sigmoid"),
    		tr("Set the sigmoid activation function"),
			SLOT(setSigmoid()));
    _setSigmoid->setShortcut(tr("Ctrl+Shift+s"));
    _nodeActions.append(_setSigmoid);
    _setLin = createAction(tr("Linear"),
    		tr("Set the linear activation function"),
			SLOT(setLin()));
    _setLin->setShortcut(tr("Ctrl+Shift+l"));
    _nodeActions.append(_setLin);
    _setStep = createAction(tr("Step"),
    		tr("Set the step activation function"),
			SLOT(setStep()));
    _setStep->setShortcut(tr("Ctrl+Shift+t"));
    _nodeActions.append(_setStep);
    _setUSigmoid = createAction(tr("Unsigned Sigmoid"),
    		tr("Set the unsigned sigmoid activation function"),
			SLOT(setUSigmoid()));
    _setUSigmoid->setShortcut(tr("Alt+Shift+s"));
    _nodeActions.append(_setUSigmoid);
    _setUGuas = createAction(tr("Unsigned Gaussian"),
    		tr("Set the unsigned Guassian activation function"),
			SLOT(setUGuas()));
    _setUGuas->setShortcut(tr("Alt+Shift+g"));
    _nodeActions.append(_setUGuas);
    _setUBoundedLinear = createAction(tr("Unsigned Bounded Linear"),
    		tr("Set the unsigned bounded linear activation function"),
			SLOT(setUBoundedLinear()));
    _setUBoundedLinear->setShortcut(tr("Alt+Shift+l"));
    _nodeActions.append(_setUBoundedLinear);


    //Add actions
    _addNode = createAction(tr("Add node"),
    		tr("Adds a node to the cppn"),
			SLOT(addNode()));
    _addNode->setShortcut(tr("n"));
    _addNodeOnConnection = createAction(tr("Add node on edge"),
    		tr("Adds a node splicing the selected edge"),
			SLOT(addNodeOnConnection()));
    _addEdge = createAction(tr("Add edge"),
    		tr("Adds an edge between two selected nodes"),
			SLOT(addEdge()));
    _addEdge->setShortcut(tr("e"));
    _twoNodeActions.append(_addEdge);

    //Arrange actions
    _alignHorizontal = createAction(tr("Align horizontal"),
    		tr("Aligns selected nodes horizontally with the first node selected."),
			SLOT(alignHorizontal()));
    _nodeActions.append(_alignHorizontal);
    _alignVertical = createAction(tr("Align vertical"),
    		tr("Aligns selected nodes vertically with the first node selected."),
			SLOT(alignVertical()));
    _nodeActions.append(_alignVertical);
    _spaceHorizontal = createAction(tr("Space horizontal"),
    		tr("Equally distributes the selected nodes between the first and last node selected, horizontally."),
			SLOT(spaceHorizontal()));
    _nodeActions.append(_spaceHorizontal);
    _spaceVertical = createAction(tr("Space vertical"),
    		tr("Equally distributes the selected nodes between the first and last node selected, vertically."),
			SLOT(spaceVertical()));
    _nodeActions.append(_spaceVertical);

    _scaleLegend = createAction(tr("Scale legend"),
    		tr("Change the size of the legend."),
			SLOT(scaleLegend()));

//    _setFavorite = createAction(tr("Favorite"), tr("Marks edge as favorite"), SLOT(setFavourite()));

//    QAction* _setSin;
//    QAction* _setCos;
//    QAction* _setGaus;
//    QAction* _setSigmoid;
//    QAction* _setLin;


    curvedLineAction = new QAction(tr("Curved line"), this);;
    curvedLineAction->setShortcut(tr("Ctrl+6"));
    curvedLineAction->setStatusTip(tr("Show all lines as curved lines"));
    curvedLineAction->setCheckable(true);
    connect(curvedLineAction, SIGNAL(triggered()), this, SLOT(setCurvedLines()));
    addAction(curvedLineAction);

    straightLineAction = new QAction(tr("Straight line"), this);
    straightLineAction->setShortcut(tr("Ctrl+7"));
    straightLineAction->setStatusTip(tr("Show all lines as straight lines"));
    straightLineAction->setCheckable(true);
    connect(straightLineAction, SIGNAL(triggered()), this, SLOT(setStraightLines()));
    addAction(straightLineAction);

    _toggleAnnotationsAction = new QAction(tr("Toggle annotations"), this);
    _toggleAnnotationsAction->setShortcut(tr("Shift+5"));
    _toggleAnnotationsAction->setStatusTip(tr("Shows or hides annotations. Add an annotation to an edge by adding :=\"<annotation>\" to the label of that edge."));
    _toggleAnnotationsAction->setCheckable(true);
    _toggleAnnotationsAction->setChecked(true);
    connect(_toggleAnnotationsAction,SIGNAL(triggered()), this, SLOT(toggleAnnotations()));
    addAction(_toggleAnnotationsAction);

    _increaseCurveOffset = new QAction(tr("Increase curve"), this);
    _increaseCurveOffset->setStatusTip(tr("Increase the curve of this line"));
    _increaseCurveOffset->setShortcut(tr("Ctrl+M"));
    connect(_increaseCurveOffset,SIGNAL(triggered()), this, SLOT(increaseCurveOffset()));
    addAction(_increaseCurveOffset);

    _decreaseCurveOffset = new QAction(tr("Decrease curve"), this);
    _decreaseCurveOffset->setStatusTip(tr("Decrease the curve of this line"));
    _decreaseCurveOffset->setShortcut(tr("Ctrl+N"));
    connect(_decreaseCurveOffset,SIGNAL(triggered()), this, SLOT(decreaseCurveOffset()));
    addAction(_decreaseCurveOffset);

    _snapshotAction = new QAction(tr("Export..."), this);
    _snapshotAction->setStatusTip(tr("Export the CPPN as an image"));
    _snapshotAction->setShortcut(tr("Ctrl+E"));
    connect(_snapshotAction,SIGNAL(triggered()), this, SLOT(snapShot()));
    addAction(_snapshotAction);

    lineModeGroup = new QActionGroup(this);;
    lineModeGroup->addAction(curvedLineAction);
    lineModeGroup->addAction(straightLineAction);
    curvedLineAction->setChecked(true);

    _colorPathAction = new QAction(tr("Color path"), this);
    _colorPathAction->setStatusTip(tr("Colors different paths in the network according to overlap"));
    connect(_colorPathAction,SIGNAL(triggered()), this, SLOT(colorPaths()));
    addAction(_colorPathAction);

//    viewGroup = new QActionGroup(this);
//    viewGroup->addAction(labelOnlyAction);
//    viewGroup->addAction(signOnlyAction);
//    viewGroup->addAction(labelAndSignAction);
//    labelOnlyAction->setChecked(true);

    circleAction = new QAction(tr("&Layers"), this);
    circleAction->setShortcut(tr("Alt+I"));
    circleAction->setStatusTip(tr("Arrange nodes in layers"));
    connect(circleAction, SIGNAL(triggered()), this, SLOT(positionNodesLayers()));
    addAction(circleAction);

    ONPAction = new QAction(tr("&ONP"), this);
    ONPAction->setShortcut(tr("Alt+O"));
    ONPAction->setStatusTip(tr("Use the Optimal Neuron Placement to arrange nodes"));
    connect(ONPAction, SIGNAL(triggered()), this, SLOT(positionNodesONP()));
    addAction(ONPAction);

    layerAction = new QAction(tr("&Circle"), this);
    layerAction->setShortcut(tr("Alt+P"));
    layerAction->setStatusTip(tr("Arrange nodes in a circle"));
    connect(layerAction, SIGNAL(triggered()), this, SLOT(positionNodesCircle()));
    addAction(layerAction);



//    connect(labelOnlyAction, SIGNAL(triggered()), this, SLOT(setLabelView()));
//    connect(signOnlyAction, SIGNAL(triggered()), this, SLOT(setSignView()));
//    connect(labelAndSignAction, SIGNAL(triggered()), this, SLOT(setLabelAndSignView()));
//
//
//    connect(labelOnlyAction, SIGNAL(triggered()), this, SLOT(setLabelView()));
//    connect(signOnlyAction, SIGNAL(triggered()), this, SLOT(setSignView()));
//    connect(labelAndSignAction, SIGNAL(triggered()), this, SLOT(setLabelAndSignView()));
//    connect(labelOnlyAction, SIGNAL(triggered()), this, SLOT(setLabelView()));
//    connect(signOnlyAction, SIGNAL(triggered()), this, SLOT(setSignView()));
//    connect(labelAndSignAction, SIGNAL(triggered()), this, SLOT(setLabelAndSignView()));


//
//    QAction* _showSignAction;
//    QAction* _showSignIfNoLabelAction;
//    QAction* _noSignAction;



    // By adding actions to the widget they will automatically be enabled or disabled
    // when the widget itself gets enabled or disabled.

//    addAction(curvedLineAction);
//
//    addAction(addNodeviewAction);
//
////    addAction(labelOnlyAction);
////    addAction(signOnlyAction);
////    addAction(labelAndSignAction);
//    addAction(circleAction);
//    addAction(layerAction);
////    addAction(deleteAction);
//    addAction(nodeLabelAction);
//    addAction(nodeModuleAction);
//    addAction(nodeLabelNoImageAction);
//    addAction(_toggleAnnotationsAction);
//    addAction(_snapshotAction);
//    addAction(_colorPathAction);
//
//    addAction(ONPAction);
//
//    addAction(_increaseCurveOffset);
//    addAction(_decreaseCurveOffset);

    setNodeSelected(false);
    setTwoNodesSelected(false);

    // Activation functions sub-menu
    _afMenu = new QMenu(tr("Activation Functions"), this);
    _afMenu->addAction(_setSin);
    _afMenu->addAction(_setCos);
    _afMenu->addAction(_setGaus);
    _afMenu->addAction(_setSigmoid);
    _afMenu->addAction(_setLin);
    _afMenu->addAction(_setStep);
    _afMenu->addAction(_setUSigmoid);
    _afMenu->addAction(_setUGuas);
    _afMenu->addAction(_setUBoundedLinear);

    // Create node menu
    nodeMenu = new QMenu(tr("Node"), this);
    nodeMenu->addAction(addNodeviewAction);
    nodeMenu->addMenu(_afMenu);

    // Create Edge menu
    edgeMenu = new QMenu(tr("Edge"), this);

}

void CppnWidget::rebuildPhenotype(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	cppn->updateNodes();
}

void CppnWidget::deleteCppn(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    clear();
    left_sorted.clear();
    right_sorted.clear();
    top_sorted.clear();
    bottom_sorted.clear();

    if(cppn){
        delete cppn;
        cppn=0;
    }
}

void CppnWidget::setCppn(QList<Node*> nodes, QList<Edge*> edges, CppnInformation* cppnInformation){
    dbg::trace trace("cppnwidget", DBG_HERE);
	//Clean previous objects
    deleteCppn();

	cppn = new Cppn;
	if(cppnInformation){
		cppn->setMinX(cppnInformation->min_x);
		cppn->setMaxX(cppnInformation->max_x);
		cppn->setMinY(cppnInformation->min_y);
		cppn->setMaxY(cppnInformation->max_y);
		cppn->setResX(cppnInformation->x_res);
		cppn->setResY(cppnInformation->y_res);
	}
	foreach(Node* node, nodes){
		addNode(node);
	}

	foreach(Edge* edge, edges){
		addEdge(edge);
	}

	connect(cppn, SIGNAL(newModularity(double)), this, SIGNAL(newModularity(double)));
	cppn->updateNodes();
}

size_t CppnWidget::getNrOfModules() const{
    dbg::trace trace("cppnwidget", DBG_HERE);
    return cppn->getNrOfModules();
}

void CppnWidget::itemMoved(MovableObject* object) {
    dbg::trace trace("cppnwidget", DBG_HERE);
    left_sorted.update(object);
    right_sorted.update(object);
    top_sorted.update(object);
    bottom_sorted.update(object);
	setSceneRect();
}

void CppnWidget::setSceneRect() {
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(left_sorted.empty()){
        left_border = -extra_space;
        right_border = extra_space;
        top_border = -extra_space;
        bottom_border = +extra_space;
    } else {
//        int height = Node::node_height + Node::footerBarSize + Node::headerBarSize;
//        int width = Node::node_width;
//        dbg::out(dbg::info, "cppnwidget") << "x: " << x_sorted.front()->pos().x() << " left: " << x_sorted.front()->boundingRect().left() << std::endl;
//        dbg::out(dbg::info, "cppnwidget") << "x: " << x_sorted.back()->pos().x() << " right: " << x_sorted.back()->boundingRect().right() << std::endl;
//        dbg::out(dbg::info, "cppnwidget") << "x: " << x_sorted.front()->pos().x() << " left: " << x_sorted.front()->boundingRect().left() << std::endl;
        left_border = left_sorted.edgeValue() - extra_space;
        right_border = right_sorted.edgeValue() + extra_space;// + width/2;
        top_border = top_sorted.edgeValue() - extra_space; // - height/2;
        bottom_border = bottom_sorted.edgeValue() + extra_space; // + height/2;
    }
    scene()->setSceneRect(QRectF(left_border,top_border,(-left_border) + right_border,(-top_border) + bottom_border));
//    scene()->setSceneRect(scene()->itemsBoundingRect());
}

void CppnWidget::wheelEvent(QWheelEvent *event) {
    dbg::trace trace("cppnwidget", DBG_HERE);
    scaleView(std::pow((double)2, event->delta() / 240.0));
}

void CppnWidget::scaleView(qreal scaleFactor) {
    dbg::trace trace("cppnwidget", DBG_HERE);
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < minimum_zoom_factor || factor > maximum_zoom_factor)
        return;

    scale(scaleFactor, scaleFactor);
}


void CppnWidget::zoomIn() {
    dbg::trace trace("cppnwidget", DBG_HERE);
    scaleView(qreal(1.2));
}

void CppnWidget::zoomOut() {
    dbg::trace trace("cppnwidget", DBG_HERE);
    scaleView(1 / qreal(1.2));
}

void CppnWidget::setZoom() {
    dbg::trace trace("cppnwidget", DBG_HERE);
    qreal factor = transform().scale(1.0, 1.0).mapRect(QRectF(0, 0, 1, 1)).width();

    // Get zoom factor
    QString title = tr("Set zoom factor");
    QString Label = tr("Choose the zoom factor for the screen.");
    bool ok = true;
    double newScaleFactor =  QInputDialog::getDouble(this, title, Label, factor, minimum_zoom_factor, maximum_zoom_factor, 4, &ok);
    if(!ok) return;

    // Set the scale of the view
    QTransform newTransform;
    newTransform.reset();
    newTransform.scale(newScaleFactor, newScaleFactor);
    setTransform(newTransform);
}

void CppnWidget::setWeight(double weight){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	blockWeightUpdates=true;
	if(edge) emit requestCommandExecution(new CommandSetWeight(edge, weight));
	blockWeightUpdates=false;
//	emit sceneModified();
}

Edge* CppnWidget::getSelectedEdge(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(scene()->selectedItems().count() != 1) return 0;
    Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
    return edge;
}

void CppnWidget::increaseCurveOffset(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge) edge->increaseCurveOffset();
	emit sceneModified();
}

void CppnWidget::decreaseCurveOffset(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge) edge->decreaseCurveOffset();
	emit sceneModified();
}

void CppnWidget::onBookendStartChanged(double bookendStart){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge){
		edge->blockSignals(true);
//		std::cout << "Bookend start: " << bookendStart <<std::endl;
		emit requestCommandExecution(new ComSetBookends(edge, bookendStart, edge->getBookendEnd(), edge->getStepsize()));
		edge->blockSignals(false);
	}
}

void CppnWidget::onBookendEndChanged(double bookendEnd){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge){
		edge->blockSignals(true);
//		std::cout << "Bookend end: " << bookendEnd <<std::endl;
		emit requestCommandExecution(new ComSetBookends(edge, edge->getBookendStart(), bookendEnd, edge->getStepsize()));
		edge->blockSignals(false);
	}

}

void CppnWidget::onBookendStepChanged(double bookendStep){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge){
		edge->blockSignals(true);
//		std::cout << "Bookend step: " << bookendStep <<std::endl;
		emit requestCommandExecution(new ComSetBookends(edge, edge->getBookendStart(), edge->getBookendEnd(), bookendStep));
		edge->blockSignals(false);
	}

}


void CppnWidget::resetWeight(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	emit requestCommandExecution(new CommandSetWeight(scene()->selectedItems()));
}

void CppnWidget::resetAllWeights(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	emit requestCommandExecution(new CommandSetWeight(scene()->items()));
}

void CppnWidget::weightUpdated(Edge* edge){
    dbg::trace trace("cppnwidget", DBG_HERE);
//	std::cout << "Update edge: " << edge->getBranch() << "_" << edge->getId() << std::endl;
	if(blockWeightUpdates) return;
	blockSignals(true);
	scene()->clearSelection();
	blockSignals(false);
	edge->setSelected(true);
}

void CppnWidget::positionNodesLayers(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	cppn->positionNodes();
	emit requestCommandExecution(new CommandSetPos(scene()->items()));
}

void CppnWidget::positionNodesCircle(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	cppn->positionNodesCircle();
	emit requestCommandExecution(new CommandSetPos(scene()->items()));
}

void CppnWidget::positionNodesONP(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	cppn->positionNodesONP();
	emit requestCommandExecution(new CommandSetPos(scene()->items()));
}

void CppnWidget::addNode(Node* node){
    dbg::trace trace("cppnwidget", DBG_HERE);
    dbg::out(dbg::info, "cppn") << "Adding node: " << node << std::endl;
    dbg::out(dbg::info, "cppn") << "       name: " << node->getName() << std::endl;

	node->setPrevPos(node->pos());
	node->setContextMenu(nodeMenu);
	node->setLabelMode(&nodeLabelMode);

	addContextMenuObject(node);
	cppn->addNode(node);
	connect(node, SIGNAL(updatePositionsRequest()), this, SLOT(updatePreviousPositions()));
	connect(node, SIGNAL(positionChanged(MovableObject*)), this, SLOT(itemMoved(MovableObject*)));
	connect(node, SIGNAL(updateRequest(Node*)), cppn, SLOT(updateNode(Node*)));
	connect(node, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
    left_sorted.add(node);
    right_sorted.add(node);
    top_sorted.add(node);
    bottom_sorted.add(node);
//	x_sorted.add(node);
//	y_sorted.add(node);
	itemMoved(node);
}

void CppnWidget::removeNode(Node* node){
    dbg::trace trace("cppnwidget", DBG_HERE);
	node->setSelected(false);
	removeContextMenuObject(node);
	cppn->removeNode(node);
	disconnect(node, SIGNAL(updatePositionsRequest()), this, SLOT(updatePreviousPositions()));
	disconnect(node, SIGNAL(positionChanged(MovableObject*)), this, SLOT(itemMoved(MovableObject*)));
	disconnect(node, SIGNAL(updateRequest(Node*)), cppn, SLOT(updateNode(Node*)));
	disconnect(node, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
    left_sorted.remove(node);
    right_sorted.remove(node);
    top_sorted.remove(node);
    bottom_sorted.remove(node);
//	x_sorted.remove(node);
//	y_sorted.remove(node);
	setSceneRect();
}


void CppnWidget::addEdge(Edge* edge){
    dbg::trace trace("cppnwidget", DBG_HERE);
	edge->setLabelMode(&labelMode);
	edge->setSignMode(&_signMode);
	edge->setLineMode(&lineMode);
	edge->setLineWidthMode(&_lineWidthMode);
	edge->setAnnotationMode(&_annotationMode);
	edge->setContextMenu(edgeMenu);

	addContextMenuObject(edge);

	cppn->addConnection(edge);
	connect(edge, SIGNAL(weightChanged(Edge*, double, bool)), cppn, SLOT(setWeight(Edge*, double, bool)));
	connect(edge, SIGNAL(weightChanged(Edge*, double, bool)), this, SLOT(weightUpdated(Edge*)));
	connect(edge, SIGNAL(requestUpdateAll()), cppn, SLOT(updateNodes()));
	connect(edge, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
	connect(edge, SIGNAL(bookendsChanged(Edge*)), this, SLOT(weightUpdated(Edge*)));
}


void CppnWidget::removeEdge(Edge* edge){
    dbg::trace trace("cppnwidget", DBG_HERE);
	edge->setSelected(false);
//	std::cout << "Removing edge: " << edge->getBranch() << "_" << edge->getId()  << " (" << edge << ")"<< std::endl;
	removeContextMenuObject(edge);
	cppn->removeConnection(edge);
	disconnect(edge, SIGNAL(weightChanged(Edge*, double, bool)), cppn, SLOT(setWeight(Edge*, double, bool)));
	disconnect(edge, SIGNAL(weightChanged(Edge*, double, bool)), this, SLOT(weightUpdated(Edge*)));
	disconnect(edge, SIGNAL(requestUpdateAll()), cppn, SLOT(updateNodes()));
	disconnect(edge, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
	disconnect(edge, SIGNAL(bookendsChanged(Edge*)), this, SLOT(weightUpdated(Edge*)));
}

void CppnWidget::scaleLegend(){
    if(!_legend) return;
    QString title = tr("Choose legend scale");
    QString Label = tr("Current scale: \n") +
            QString::number(_legend->scale(), 'f', 1) +
            tr(".");
    bool ok = true;
    qreal newScale =  QInputDialog::getDouble(this, title, Label, _legend->scale(), 0.0001, 10000.0, 4, &ok);
    if(!ok) return;
    _legend->setScale(newScale);
    itemMoved(_legend);
}

Legend* CppnWidget::getLegend(){
    return _legend;
}

void CppnWidget::addLegend(const QList<Label*>& labels){
    dbg::trace trace("cppnwidget", DBG_HERE);
    _legend = new Legend();
    _legend->setLabels(labels);
    _legend->setPos(getLeftBorder() + extra_space, getTopBorder() + extra_space);
    _legend->setPrevPos(_legend->pos());

    addContextMenuObject(_legend);
    connect(_legend, SIGNAL(updatePositionsRequest()), this, SLOT(updatePreviousPositions()));
    connect(_legend, SIGNAL(positionChanged(MovableObject*)), this, SLOT(itemMoved(MovableObject*)));
    left_sorted.add(_legend);
    right_sorted.add(_legend);
    top_sorted.add(_legend);
    bottom_sorted.add(_legend);
    itemMoved(_legend);
}


void CppnWidget::removeLegend(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    removeContextMenuObject(_legend);
    disconnect(_legend, SIGNAL(updatePositionsRequest()), this, SLOT(updatePreviousPositions()));
    disconnect(_legend, SIGNAL(positionChanged(MovableObject*)), this, SLOT(itemMoved(MovableObject*)));
    left_sorted.remove(_legend);
    right_sorted.remove(_legend);
    top_sorted.remove(_legend);
    bottom_sorted.remove(_legend);
    setSceneRect();
    delete _legend;
    _legend = 0;
}

void CppnWidget::applyLabel(Label* label){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(scene()->selectedItems().count() == 0) return;
	emit requestCommandExecution(new CommandLabelObject(scene()->selectedItems(), label));
	emit sceneModified();
}

void CppnWidget::addNodeView(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	emit requestAddNodeview(scene()->selectedItems());
}

void CppnWidget::updateSelection(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	nodeSelected = false;
	edgeSelected = false;

	const QList<QGraphicsItem *>& selected = scene()->selectedItems();
	int nodeCount = 0;
	foreach(QGraphicsItem* item, selected){
		if(qgraphicsitem_cast<Node*>(item)){
			nodeSelected = true;
			nodeCount += 1;
		}
		if(qgraphicsitem_cast<Edge*>(item)) edgeSelected = true;
		if(edgeSelected && nodeSelected) break;
	}
	_twoNodesSelected = (nodeCount == 2 && selected.size() == 2);

	if(scene()->selectedItems().count() == 1){
		if(edgeSelected) emit edgeUpdated(qgraphicsitem_cast<Edge*>(selected.front()));
		if(nodeSelected){
		    _firstNodeSelected = qgraphicsitem_cast<Node*>(selected.front());
		    emit nodeUpdated(_firstNodeSelected);
		}
	} else {
		emit edgeUpdated(0);
	}

	if(!nodeSelected){
	    _firstNodeSelected = 0;
	}

	emit labelableObjectSelected(edgeSelected || nodeSelected);

	setNodeSelected(nodeSelected);
	setTwoNodesSelected(_twoNodesSelected);
	setEdgeSelected(edgeSelected);
}

void CppnWidget::updatePreviousPositions(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	emit requestCommandExecution(new CommandSetPos(scene()->selectedItems()));
	foreach(QGraphicsItem* item, scene()->selectedItems()){
	    MovableObject* object = util::multiCast<MovableObject*, Node*, Legend*>(item);
		if(object) itemMoved(object);
	}
}

void CppnWidget::setEdgeShowSign(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    _signMode = Edge::showSign;
    update();
}

void CppnWidget::setEdgeShowSignIfNoLabel(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    _signMode = Edge::showSignIfNoLabel;
    update();
}

void CppnWidget::setEdgeNoSign(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    _signMode = Edge::noSign;
    update();
}

void CppnWidget::setEdgeShowLabel(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    labelMode = Edge::showLabel;
    update();
}

void CppnWidget::setEdgeNoLabel(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    labelMode = Edge::noLabel;
    update();
}

void CppnWidget::setCurvedLines(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	lineMode = Edge::curved;
	update();
}

void CppnWidget::setStraightLines(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	lineMode = Edge::straight;
	update();
}

void CppnWidget::setLabelView(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	labelMode = Edge::onlyLabels;
	update();
}

void CppnWidget::setSignView(){
    dbg::trace trace("cppnwidget", DBG_HERE);
 	labelMode = Edge::onlyConnectionSign;
 	update();
}

void CppnWidget::setLabelAndSignView(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	labelMode = Edge::both;
	update();
}

void CppnWidget::setNodeLabelView(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	nodeLabelMode = Node::labels;
	updateAll();
}

void CppnWidget::setNodeModuleView(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	nodeLabelMode = Node::modules;
	updateAll();
}

void CppnWidget::setNodeLabelNoImageView(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    nodeLabelMode = Node::labelsNoImage;
    updateAll();
}

void CppnWidget::setEdgeWeightWidth(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    _lineWidthMode = Edge::weightWidth;
    update();
}

void CppnWidget::setEdgeFixedWidth(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    _lineWidthMode = Edge::fixedWidth;
    update();
}

void CppnWidget::setShowAnnotations(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    _annotationMode = Edge::showAnnotation;
    update();
}

void CppnWidget::setHideAnnotations(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    _annotationMode = Edge::hideAnnotation;
    update();
}

void CppnWidget::toggleAnnotations(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(_annotationMode == Edge::showAnnotation){
        setHideAnnotations();
    } else {
        setShowAnnotations();
    }
}

void CppnWidget::changeEvent(QEvent* event){
    dbg::trace trace("cppnwidget", DBG_HERE);
	switch(event->type ()){
	case(QEvent::EnabledChange):
		foreach(QAction* action, actions()){
			action->setEnabled(isEnabled());
		}
		if(isEnabled()){
			setNodeSelected(nodeSelected);
			setTwoNodesSelected(_twoNodesSelected);
			setEdgeSelected(edgeSelected);
		}
	break;
	default:
		//Nix
	break;
	}
}

void CppnWidget::setNodeSelected(bool selected){
    dbg::trace trace("cppnwidget", DBG_HERE);
	nodeSelected = selected;
	foreach(QAction* action, _nodeActions){
		action->setEnabled(selected);
	}
}

void CppnWidget::setTwoNodesSelected(bool selected){
    dbg::trace trace("cppnwidget", DBG_HERE);
    _twoNodesSelected = selected;
	foreach(QAction* action, _twoNodeActions){
		action->setEnabled(selected);
	}
}

void CppnWidget::setEdgeSelected(bool selected){
    dbg::trace trace("cppnwidget", DBG_HERE);
	edgeSelected = selected;
}

void CppnWidget::saveImage(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(nodeSelected){
		std::vector<Node*> nodes;
		foreach(QGraphicsItem* item, scene()->selectedItems()){
			Node* node = qgraphicsitem_cast<Node*>(item);
			if(node){
				nodes.push_back(node);
			}
		}
		util::saveBatch(this, nodes);
	}
}

void CppnWidget::snapShot(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    //Ask filename
    QString newFileName = util::getSupportedFilename(this, tr("Save network snapshot"));
    if(newFileName.isEmpty()) return;

    //Calculate network dimensions
    setSceneRect();
    QRectF rect = this->scene()->sceneRect();

    //Get resolution multiplier
    QString title = tr("Choose resolution multiplier");
    QString Label = tr("The final resolution is the dimensions of the\n") +
            tr("network multiplied by the resolution multiplier.\n") +
            tr("The dimensions of the current network are: ") +
            QString::number(rect.width(), 'f', 1) +
            tr(" by ") +
            QString::number(rect.height(), 'f', 1) +
            tr(".");
    bool ok = true;
    double resolutionMultiplier =  QInputDialog::getDouble(this, title, Label, 2.0, 0.0001, 10000.0, 4, &ok);
    if(!ok) return;

    //Update every item to ensure painting the most recent iteration of each item
    foreach(QGraphicsItem* item, this->scene()->items()){
        item->update();
    }

    //Create the snapshot
    QPixmap pixmap(rect.width()*resolutionMultiplier, rect.height()*resolutionMultiplier);
    QPainter painter(&pixmap);
    painter.fillRect (0, 0, rect.width()*resolutionMultiplier, rect.height()*resolutionMultiplier, QColor(255,255,255));
//    QBrush brush(QColor(255,255,255));
//    painter.setBackground(brush);
//    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setRenderHint( QPainter::Antialiasing );
    this->scene()->setBackgroundBrush(Qt::white);
    this->scene()->render(&painter);

    pixmap.save(newFileName);
}

/***************************
 * Set activation function *
 ***************************/
void CppnWidget::setSin(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    setActivationFunction(XML_SIN);
}

void CppnWidget::setCos(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    setActivationFunction(XML_COS);
}

void CppnWidget::setGaus(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    setActivationFunction(XML_GAUSSIAN);
}

void CppnWidget::setSigmoid(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    setActivationFunction(XML_SIGMOID);
}

void CppnWidget::setLin(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    setActivationFunction(XML_LINEAR);
}

void CppnWidget::setStep(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    setActivationFunction(XML_STEP);
}

void CppnWidget::setUGuas(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    setActivationFunction(XML_U_GUASSIAN);
}

void CppnWidget::setUSigmoid(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    setActivationFunction(XML_U_SIGMOID);
}

void CppnWidget::setUBoundedLinear(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    setActivationFunction(XML_U_BOUNDED_LINEAR);
}

void CppnWidget::setActivationFunction(std::string activationFunction){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(!nodeSelected) return;
    QList<Node*> nodes = _getSelectedNodes();
    CommandChangeActivation* command = new CommandChangeActivation();
    command->init(this, nodes, activationFunction);
    if(command->isOk()) emit requestCommandExecution(command);
}

/*************************
 ** Add node functions ***
 *************************/
void CppnWidget::addNode(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(edgeSelected && scene()->selectedItems().size() == 1){
    	addNodeOnConnection();
    } else {
    	QPoint rawPos = this->mapFromGlobal(QCursor::pos());
    	QPointF mousePosition = this->mapToScene(rawPos);
    	std::string branch = "0";
    	std::string id = cppn->getNextId();
    	std::string type = XML_TYPE_HIDDEN;
    	std::string activation = XML_LINEAR;
    	Node* node = new Node(branch, id, type, activation);
    	node->setPos(mousePosition);
    	ComAddRemoveObject* command = new ComAddRemoveObject();
    	command->init(this);
    	command->addObject(node);
    	command->setText("Add node");
    	if(command->isOk()) emit requestCommandExecution(command);
    }
}

void CppnWidget::addNodeOnConnection(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(!edgeSelected || scene()->selectedItems().size() > 1 || scene()->selectedItems().empty()) return;
    QGraphicsItem* item = scene()->selectedItems().back();
    Edge* edge = qgraphicsitem_cast<Edge*>(item);
    QPointF position = (edge->sourceNode()->pos() + edge->destNode()->pos())/2;

    std::string branch = "0";
    std::string id = cppn->getNextId();
    std::string type = XML_TYPE_HIDDEN;
    std::string activation = XML_LINEAR;
    Node* node = new Node(branch, id, type, activation);
    node->setPos(position);
    node->setPrevPos(position);

    Edge* newEdge1 = new Edge(branch, id, edge->sourceNode(), node, edge->getWeight(), edge->getOriginalWeight());
    Edge* newEdge2 = new Edge(branch, id, node, edge->destNode(), 1, 1);

    ComAddRemoveObject* command = new ComAddRemoveObject();
    command->init(this);
    command->removeObject(edge);
    command->addObject(node);
    command->addObject(newEdge1);
    command->addObject(newEdge2);
    command->setText("Add node on edge");
    if(command->isOk()) emit requestCommandExecution(command);
}


void CppnWidget::addEdge(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(!nodeSelected || scene()->selectedItems().size() != 2) return;
    QGraphicsItem* item1 = scene()->selectedItems().front();
    QGraphicsItem* item2 = scene()->selectedItems().back();
    Node* node1 = qgraphicsitem_cast<Node*>(item1);
    Node* node2 = qgraphicsitem_cast<Node*>(item2);
    if(!node1 || !node2) return;

    //Make sure we are always connecting from the first node selected
    if (node2 == _firstNodeSelected){
        node2 = node1;
        node1 = _firstNodeSelected;
    }

    std::string branch = "0";
    std::string id = cppn->getNextId();
    Edge* newEdge = new Edge(branch, id, node1, node2, 0, 0);

    if(!cppn->connectionIsOkay(newEdge)){
//        std::cout << "And now it crashes?" << std::endl;
        newEdge->setSourceNode(node2);
        newEdge->setTargetNode(node1);
        if(!cppn->connectionIsOkay(newEdge)){
            std::cerr << "ERROR: Could not add connection, connection is illegal either way" << std::endl;
            return;
        }
    }

    ComAddRemoveObject* command = new ComAddRemoveObject();
    command->init(this);
    command->addObject(newEdge);
    command->setText("Add edge");
    if(command->isOk()) emit requestCommandExecution(command);
}

/*************************
 *** Arrange functions ***
 *************************/
void CppnWidget::alignHorizontal(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(!nodeSelected)  return;
    QList<Node*> nodes = _getSelectedNodes();
    QPointF referencePos;
    if(_firstNodeSelected){
        referencePos = _firstNodeSelected->pos();
    } else {
        referencePos = nodes.first()->pos();
    }

    qreal yPos = referencePos.y();
    foreach(Node* node, nodes){
        QPointF currentPos = node->pos();
        currentPos.setY(yPos);
        node->setPos(currentPos);
    }

    CommandSetPos* command = new CommandSetPos(nodes);
    if(command->isOk()) emit requestCommandExecution(command);
}

void CppnWidget::alignVertical(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(!nodeSelected)  return;
    QList<Node*> nodes = _getSelectedNodes();
    QPointF referencePos;
    if(_firstNodeSelected){
        referencePos = _firstNodeSelected->pos();
    } else {
        referencePos = nodes.first()->pos();
    }
    qreal xPos = referencePos.x();
    foreach(Node* node, nodes){
        QPointF currentPos = node->pos();
        currentPos.setX(xPos);
        node->setPos(currentPos);
    }

    CommandSetPos* command = new CommandSetPos(nodes);
    if(command->isOk()) emit requestCommandExecution(command);
}

void CppnWidget::spaceHorizontal(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(!nodeSelected)  return;
    QList<Node*> nodes = _getSelectedNodes();
    if(nodes.size() < 3) return;

    qSort(nodes.begin(), nodes.end(), xPosLessThan);
    qreal xPosMin = nodes.first()->pos().x();
    qreal xPosMax = nodes.last()->pos().x();

    qreal delta = (xPosMax - xPosMin) / qreal(nodes.size()-1);
    qreal currentXPos = xPosMin;

    foreach(Node* node, nodes){
        QPointF currentPos = node->pos();
        currentPos.setX(currentXPos);
        node->setPos(currentPos);
        currentXPos += delta;
    }

    CommandSetPos* command = new CommandSetPos(nodes);
    if(command->isOk()) emit requestCommandExecution(command);
}


void CppnWidget::spaceVertical(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    if(!nodeSelected)  return;
    QList<Node*> nodes = _getSelectedNodes();
    if(nodes.size() < 3) return;

    qSort(nodes.begin(), nodes.end(), yPosLessThan);
    qreal yPosMin = nodes.first()->pos().y();
    qreal yPosMax = nodes.last()->pos().y();
    qreal delta = (yPosMax - yPosMin) / qreal(nodes.size()-1);
    qreal currentYPos = yPosMin;

    foreach(Node* node, nodes){
        QPointF currentPos = node->pos();
        currentPos.setY(currentYPos);
        node->setPos(currentPos);
        currentYPos += delta;
    }

    CommandSetPos* command = new CommandSetPos(nodes);
    if(command->isOk()) emit requestCommandExecution(command);
}

//void CppnWidget::setFavourite(){
//    if(scene()->selectedItems().empty()) return;
//    foreach(QGraphicsItem* item, scene()->selectedItems()){
//        Edge* edge = qgraphicsitem_cast<Edge*>(item);
//        if(edge){
//            ++_current_favourite_index;
//            edge->setFavourite(_current_favourite_index);
//        }
//    }
//}


void CppnWidget::deleteObjects(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	ComAddRemoveObject* command = new ComAddRemoveObject(this, scene()->selectedItems(), false);
	if(command->isOk()) emit requestCommandExecution(command);
}


void CppnWidget::deselectItems(){
    dbg::trace trace("cppnwidget", DBG_HERE);
	scene()->clearSelection();
}

void CppnWidget::flash(bool flashOn){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge) edge->flash(flashOn);
}

void CppnWidget::mousePressEvent(QMouseEvent * event){
    dbg::trace trace("cppnwidget", DBG_HERE);
	if(firstClick && scene()->selectedItems().count() > 0){
		return;
	}
	QGraphicsView::mousePressEvent(event);
}

void CppnWidget::mouseReleaseEvent(QMouseEvent * event){
    dbg::trace trace("cppnwidget", DBG_HERE);
//	std::cout << "Mouse release" << std::endl;
	if(firstClick && scene()->selectedItems().count() > 0){
//		event->accept();
		firstClick = false;
		return;
	}
	ContextMenuGraphicsView::mouseReleaseEvent(event);
}

void CppnWidget::focusOutEvent ( QFocusEvent * event){
    dbg::trace trace("cppnwidget", DBG_HERE);
	firstClick = true;
	ContextMenuGraphicsView::focusOutEvent (event);
}


void CppnWidget::findUnlabeledEdge(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    foreach(Edge* edge, cppn->getEdges()){
        if(!edge->hasLabel()){
            deselectItems();
            edge->setSelected(true);
            return;
        }
    }
}

void CppnWidget::findEdgeById(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    bool ok;
    QString title = tr("Find edge by ID (regular expressions are enabled)");
    QString text = QInputDialog::getText(this, title, tr("Edge ID:"), QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty()){
        QRegExp expr(text);
        foreach(Edge* edge, cppn->getEdges()){
            QString id =  util::toQString(edge->getId())  + "_" + util::toQString(edge->getBranch());
            if(expr.exactMatch(id)){
                deselectItems();
                edge->setSelected(true);
                return;
            }
        }
    }
}

QList<Node*> CppnWidget::_getSelectedNodes(){
    dbg::trace trace("cppnwidget", DBG_HERE);
    QList<Node*> nodes;
    foreach(QGraphicsItem* item, scene()->selectedItems()){
        Node* node = qgraphicsitem_cast<Node*>(item);
        if(node) nodes.append(node);
    }
    return nodes;
}


void CppnWidget::colorPaths(){
    bool ok;
    int i = QInputDialog::getInt(this, tr("Set threshold"),
            tr("Threshold:"), 0, 0, 2147483647, 1, &ok);
    if (ok)
        cppn->colorPaths(i, 0);
    float f = QInputDialog::getDouble(this, tr("Set percentage threshold"),
            tr("Threshold:"), 0, 0, 1, 10, &ok);
    if (ok)
        cppn->colorPaths(-1, f);

}
