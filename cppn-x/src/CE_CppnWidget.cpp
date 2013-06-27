/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "CE_CppnWidget.h"


#include <QtGui>

#include <cmath>
#include "CE_Util.h"


const double GraphWidget::left_border_min = -400.0;
const double GraphWidget::right_border_min = 400.0;
const double GraphWidget::top_border_min = -600.0;
const double GraphWidget::bottom_border_min = 600.0;
const double GraphWidget::extra_space = 80;

//! [0]
GraphWidget::GraphWidget(Window *window, QWidget *parent)
    : QGraphicsView(parent), timerId(0), sliderValue(0), selectedEdge(0), selectedNode(0), par_window(window)
{

	cppn = 0;
//
    left_border = left_border_min;
    right_border = right_border_min;
    top_border = top_border_min;
    bottom_border = bottom_border_min;

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setSceneRect(left_border, top_border, (-left_border)+right_border, (-top_border)+bottom_border);
//    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
//    connect(scene, SIGNAL(selectionChanged()), this, SLOT(selectEdge()));
    setScene(scene);


    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setDragMode(ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);
//
//
//    setCacheMode(CacheNone);
//    setViewportUpdateMode(BoundingRectViewportUpdate);
//    setViewportUpdateMode(MinimalViewportUpdate);
//    setViewportUpdateMode(BoundingRectViewportUpdate);
//
//

    setTransformationAnchor(AnchorUnderMouse);
//    setTransformationAnchor(NoAnchor);



//! [0]

}
//! [1]


void GraphWidget::updateAll(){
	foreach(QGraphicsItem* item, scene()->items()){
		item->update();
	}
}

bool GraphWidget::load(std::string filename)
{
	par_window->clearColorButtons();
	CppnParser parser(filename, this);
	cppn = parser.parse();
	par_window->setCppn(cppn);
	//		cppn_phen = shared_ptr<NEAT::FastNetwork<double> >(new NEAT::FastNetwork<double>(cppn->spawnFastPhenotypeStack<double>()));

	x_sorted.clear();
	y_sorted.clear();
	scene()->clear();
	for(size_t i=0; i< cppn->getNrOfNodes();i++){
//		std::cout << "Node: " << i << " " << cppn->getNode(i) << std::endl;
		scene()->addItem(cppn->getNode(i));
	}
	for(size_t i=0; i< cppn->getNrOfEdges();i++){
//		std::cout << "Edge: " << i << " " << cppn->getEdge(i) << std::endl;
		scene()->addItem(cppn->getEdge(i));
	}


//	std::cout << "Update nodes: " << std::endl;
	cppn->updateNodes();

	if(cppn->getNewFile()) cppn->positionNodes();

	for(size_t i=0; i< cppn->getNrOfNodes();i++){
		addNode(cppn->getNode(i));
	}

	for(size_t i=0; i<x_sorted.size(); i++){
		x_sorted[i]->x_index = i;
	}

	for(size_t i=0; i<y_sorted.size(); i++){
		y_sorted[i]->y_index = i;
	}

	for(size_t i=0; i< cppn->getNrOfNodes();i++){
		itemMoved(cppn->getNode(i));
	}

	return true;
}

bool GraphWidget::save(std::string filename)
{

	CppnWriter writer(filename);
	writer.write(cppn);
	return true;
}



//! [2]

//inline bool GraphWidget::safeCheck(std::vector<Node*>::iterator it, Node* node){
//	if(it == x_sorted.begin()) return false;
//	return (*(it-1))->pos().x() > node->pos().x();
//}

void GraphWidget::itemMoved(Node * node)
{


	size_t x_index = node->x_index;
//	std::cout << "x_index: " << x_index << std::endl;
	while(x_index > 0 ? x_sorted[x_index-1]->pos().x() > node->pos().x() :  false){
		x_sorted[x_index] = x_sorted[x_index-1];
		x_sorted[x_index]->x_index = x_index;
		x_sorted[x_index-1]=node;
		x_index--;
	}
//	std::cout << "x_index: " << x_index << std::endl;
	while(x_index < (x_sorted.size()-1) ? x_sorted[x_index+1]->pos().x() < node->pos().x() :  false){
		x_sorted[x_index] = x_sorted[x_index+1];
		x_sorted[x_index]->x_index = x_index;
		x_sorted[x_index+1]=node;
		x_index++;
	}
//	std::cout << "x_index: " << x_index << std::endl;

	size_t y_index = node->y_index;
//	std::cout << "y_index: " << y_index << std::endl;
	while(y_index > 0 ? y_sorted[y_index-1]->pos().y() > node->pos().y() :  false){
		y_sorted[y_index] = y_sorted[y_index-1];
		y_sorted[y_index]->y_index = y_index;
		y_sorted[y_index-1]=node;
		y_index--;
	}
//	std::cout << "y_index: " << y_index << std::endl;
	while(y_index < (y_sorted.size()-1) ? y_sorted[y_index+1]->pos().y() < node->pos().y() :  false){
		y_sorted[y_index] = y_sorted[y_index+1];
		y_sorted[y_index]->y_index = y_index;
		y_sorted[y_index+1]=node;
		y_index++;
	}
//	std::cout << "y_index: " << y_index << std::endl;

	node->x_index = x_index;
	node->y_index = y_index;

	//			std::cout << (*it)->pos().x() << std::endl;



//	for(std::vector<Node*>::iterator it=x_sorted.begin(); it != x_sorted.end(); it++){
//		std::cout << (*it)->pos().x()  <<std::endl;
//	}

	if(x_index == 0){
		left_border = std::min(node->pos().x() - extra_space, left_border_min);
	} else if(x_index == (x_sorted.size()-1)) {
		right_border = std::max(node->pos().x() + extra_space, right_border_min);
	}

	if(y_index == 0){
		top_border = std::min(node->pos().y() - extra_space, top_border_min);
	} else if(y_index == (y_sorted.size()-1)) {
		bottom_border = std::max(node->pos().y() + extra_space, bottom_border_min);
	}

//	std::cout << left_border << std::endl;
//	std::cout << (-left_border) + right_border << std::endl;

	scene()->setSceneRect(QRectF(left_border,top_border,(-left_border) + right_border,(-top_border) + bottom_border));
//	scene()->setSceneRect(QRectF());
	par_window->setWindowModified(true);
}
//! [2]

//! [3]
//void GraphWidget::keyPressEvent(QKeyEvent *event)
//{
//    switch (event->key()) {
//    case Qt::Key_Up:
//
//        break;
//    case Qt::Key_Down:
//
//        break;
//    case Qt::Key_Left:
//
//        break;
//    case Qt::Key_Right:
//
//        break;
//    case Qt::Key_Plus:
//        zoomIn();
//        break;
//    case Qt::Key_Minus:
//        zoomOut();
//        break;
//    case Qt::Key_Space:
//    case Qt::Key_Enter:
////        shuffle();
//        break;
//    default:
//        QGraphicsView::keyPressEvent(event);
//    }
//}
//! [3]

//! [4]
void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    if(sliderValue <101){
    	setValue(sliderValue+1);
    	QPixmap test = QPixmap::grabWindow(par_window->winId());
    	std::string name = "/Users/joost/test" + util::toString(sliderValue) + ".jpg";
    	test.save(name.c_str());
    }else{
    	killTimer(timerId);
    	timerId=0;
    }

//    std::cout <<  "Timer: "<< sliderValue << std::endl;


//    QList<Node *> nodes;
//    foreach (QGraphicsItem *item, scene()->items()) {
//        if (Node *node = qgraphicsitem_cast<Node *>(item))
//            nodes << node;
//    }
//
////    foreach (Node *node, nodes)
////        node->calculateForces();
//
//    bool itemsMoved = false;
//    foreach (Node *node, nodes) {
//        if (node->advance())
//            itemsMoved = true;
//    }
//
//    if (!itemsMoved) {
//        killTimer(timerId);
//        timerId = 0;
//    }
}
//! [4]

//! [5]
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(std::pow((double)2, event->delta() / 240.0));
}
//! [5]

//! [6]
//void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
//{
//    Q_UNUSED(rect);
//
//    // Shadow
////    QRectF sceneRect = this->sceneRect();
////    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
////    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
////    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
////	painter->fillRect(rightShadow, Qt::darkGray);
////    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
////	painter->fillRect(bottomShadow, Qt::darkGray);
////
////    // Fill
////    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
////    gradient.setColorAt(0, Qt::white);
////    gradient.setColorAt(1, Qt::white);
////    //gradient.setColorAt(1, Qt::lightGray);
////    painter->fillRect(rect.intersect(sceneRect), gradient);
////    painter->setBrush(Qt::NoBrush);
////    painter->drawRect(sceneRect);
////#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5)
//    // Text
////    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4, sceneRect.width() - 4, sceneRect.height() - 4);
////    QString message(tr("Click and drag the nodes around, and zoom with the mouse wheel or the '+' and '-' keys"));
////
////    QFont font = painter->font();
////    font.setBold(true);
////    font.setPointSize(14);
////    painter->setFont(font);
////    painter->setPen(Qt::lightGray);
////    painter->drawText(textRect.translated(2, 2), message);
////    painter->setPen(Qt::black);
////    painter->drawText(textRect, message);
////#endif
//}
//! [6]

//! [7]
void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
//! [7]

//void GraphWidget::shuffle()
//{
//    foreach (QGraphicsItem *item, scene()->items()) {
//        if (qgraphicsitem_cast<Node *>(item))
//            item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
//    }
//}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

//void GraphWidget::edgeSelected(int id, Edge* selectedEdge){
////	std::cout << id << std::endl;
//
//}

void GraphWidget::setValue(int value){

	if(sliderValue!=value){
		double newWeight = (float(value))*0.06-3;
		//std::cout << "Set Value: "<< newWeight << std::endl;
		emit sliderValueChangedF(newWeight);
	}

	sliderValue=value;
}

void GraphWidget::setValueF(double weight){
//	std::cout <<  "Set weight f: "<< weight << std::endl;
	int value = round(((weight+3)/6)*100);
//	std::cout <<  "Set Value f: "<< value << std::endl;
	sliderValue=value;
	emit sliderValueChanged(value);

	if(selectedEdge){
	//	selectedEdge->setWeight(weight);
	//	selectedEdge->update();
		cppn->setWeight(selectedEdge, weight);
//		cppn_phen->getLink(edgeId)->weight=weight;
//		updateNodes();
	}
}

void GraphWidget::resetWeight(){
	if(selectedEdge){
		killTimer(timerId);
		timerId=0;
		emit sliderValueChangedF(selectedEdge->getOriginalWeight());
	}
}

void GraphWidget::resetAllWeights(){
	killTimer(timerId);
	for(size_t i=0; i< cppn->getNrOfEdges();i++){
		cppn->setWeight(cppn->getEdge(i), cppn->getEdge(i)->getOriginalWeight(), false);
	}
	cppn->updateNodes();


	if(selectedEdge){
		emit sliderValueChangedF(selectedEdge->getOriginalWeight());
	}
}

void GraphWidget::scanWeight(){
	if(!timerId){
		timerId = startTimer(10);
		setValue(0);
	} else {
		killTimer(timerId);
		timerId=0;
	}
}


//void GraphWidget::selectEdge(){
//	killTimer(timerId);
//	timerId=0;
//
//	if(scene()->selectedItems().count() == 0){
//		selectedEdge = NULL;
//		selectedNode = NULL;
//		par_window->showScrubberBar(false);
////		par_window->showColorBar(false);
//		return;
//	}
//
//	selectedEdge = qgraphicsitem_cast<Edge*> ( scene()->selectedItems().front());
//	if(selectedEdge){
//		emit sliderValueChangedF(selectedEdge->getWeight());
//		selectedNode = NULL;
//		par_window->showScrubberBar(true);
////		par_window->showColorBar(false);
//
//	} else {
//		selectedNode = qgraphicsitem_cast<Node*> ( scene()->selectedItems().front());
//		selectedEdge = NULL;
////		par_window->showColorBar(true);
//		par_window->showScrubberBar(false);
//
//	}
//
//
//}

Window* GraphWidget::getWindow(){
	return par_window;
}

void GraphWidget::positionNodesLayers(){
	cppn->positionNodes();
}


void GraphWidget::positionNodesCircle(){
	cppn->positionNodesCircle();
}

void GraphWidget::addNode(Node* node){

	std::vector<Node*>::iterator it;
	for(it=x_sorted.begin(); it != x_sorted.end(); it++){
		if((*it)->pos().x() > node->pos().x()){
			x_sorted.insert(it, node);
			break;
		}
	}
	if(it == x_sorted.end()) x_sorted.push_back(node);

	for(it=y_sorted.begin(); it != y_sorted.end(); it++){
		if((*it)->pos().y() > node->pos().y()){
			y_sorted.insert(it, node);
			return;
		}
	}

	y_sorted.push_back(node);
}

int GraphWidget::getGeneration(int min, int max){
	bool ok = true;

	std::string text = "This files contains multiple generations from " + util::toString(min) + " to " + util::toString(max) + "\nPlease select a generation to display.";

	if(!ok) throw JGTL::LocatedException("Parsing canceled.");
	return QInputDialog::getInt(this, tr("Select Generation"), tr(text.c_str()), 0, min, max, 1, &ok);
}

void GraphWidget::warning(std::string message){
	QMessageBox msgBox(QMessageBox::Warning,tr("Warning"),tr(message.c_str()), QMessageBox::Ok, this);
	msgBox.exec();
}

//QSize GraphWidget::minimumSize() const{
//	std::cout << "min Used" << std::endl;
//	return QSize(0,0);
//}
//
//QSize GraphWidget::minimumSizeHint() const{
//	std::cout << "hint Used" << std::endl;
//	return QSize(1,1);
//}


