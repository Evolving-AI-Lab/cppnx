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

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
//#include <JGTL_LocatedException.h>
#include <QtGui/QGraphicsView>
#include <QActionGroup>
#include <map>
#include "CE_Defines.h"
//#include "CE_Label.h"
//#include "CE_Window.h"

#include "CE_CommandSetPos.h"
#include "CE_CommandLabelObject.h"
#include "CE_CommandSetWeight.h"
#include "CX_ComAddRemoveObject.h"
#include "CX_ContextMenuGraphicsView.h"
#include "CX_SortedNodesList.h"


class Node;
class Edge;
class Cppn;
//class Window;

//! [0]
class CppnWidget : public ContextMenuGraphicsView
{
    Q_OBJECT

public:
    CppnWidget(QWidget* widget = 0);

    void setCppn(QList<Node*> nodes, QList<Edge*> edges);
    Cppn* getCppn(){
    	return cppn;
    }

    QAction* getAddNodeviewAction(){
    	return addNodeviewAction;
    }

    QAction* getLabelViewAction(){
    	return labelOnlyAction;
    }

    QAction* getSignViewAction(){
    	return signOnlyAction;
    }

    QAction* getLabelAndSignViewAction(){
    	return labelAndSignAction;
    }

    QAction* getCircleAction(){
    	return circleAction;
    }

    QAction* getLayerAction(){
    	return layerAction;
    }

    QAction* getONPAction(){
    	return ONPAction;
    }

    QAction* getNodeLabelAction(){
    	return nodeLabelAction;
    }

    QAction* getNodeModuleAction(){
    	return nodeModuleAction;
    }

    QAction* getCurvedLineAction(){
    	return curvedLineAction;
    }

    QAction* getStraightLineAction(){
    	return straightLineAction;
    }


    QActionGroup* getViewGroup(){
    	return viewGroup;
    }

    QActionGroup* getNodeViewGroup(){
    	return nodeViewGroup;
    }

    QMenu* getEdgeMenu(){
    	return edgeMenu;
    }

    QMenu* getNodeMenu(){
    	return nodeMenu;
    }

    bool getNodeSelected(){
    	return nodeSelected;
    }

    bool getEdgeSelected(){
    	return edgeSelected;
    }

    void removeNode(Node* node);
    void removeEdge(Edge* edge);

public slots:
	// Zoom functions
    void zoomIn();
    void zoomOut();

    //Weight functions
    void setWeight(double value);
    void resetWeight();
    void resetAllWeights();
    void weightUpdated(Edge* edge);
    void onBookendStartChanged(double);
    void onBookendEndChanged(double);
    void onBookendStepChanged(double);

    //Position functions
    void positionNodesLayers();
    void positionNodesCircle();
    void positionNodesONP();

    //Add node/edge functions
    void addNode(Node* node);
    void addEdge(Edge* edge);

    //Apply label function
    void applyLabel(Label* label);

    //Add nodeview function
    void addNodeView();

    //Set view functions
    void setCurvedLines();
    void setStraightLines();

    void setLabelView();
    void setSignView();
    void setLabelAndSignView();

    void setNodeLabelView();
    void setNodeModuleView();

    //Update functions
    void updateSelection();
//    void updateAll();
    void updatePreviousPositions();
    void rebuildPhenotype();

    void itemMoved(Node * node);

    void saveImage();

    void deleteObjects();

    void deselectItems();

    void flash(bool flashOn);
//    void ContextMenuEvent(SelectableObject* object, bool begin);

signals:
//	void requestCommandExecution(QUndoCommand*);
	void requestAddNodeview(QList<QGraphicsItem*>);
	void edgeUpdated(Edge*);
	void nodeUpdated(Node*);
	void labelableObjectSelected(bool);
	void newModularity(double);

protected:
    void wheelEvent(QWheelEvent* event);
    void scaleView(qreal scaleFactor);
    void changeEvent(QEvent* event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void focusOutEvent ( QFocusEvent * event);


private:
    void setNodeSelected(bool selected);
    void setEdgeSelected(bool selected);
    void setSceneRect();

    Cppn* cppn;

    //Actions
    QAction* addNodeviewAction;
    QAction *labelOnlyAction;
    QAction *signOnlyAction;
    QAction *labelAndSignAction;

    //Positioning actions
    QAction* layerAction;
    QAction* ONPAction;
    QAction* circleAction;
    QActionGroup* viewGroup;

    QAction* nodeLabelAction;
    QAction* nodeModuleAction;
    QActionGroup* nodeViewGroup;

    QAction* curvedLineAction;
    QAction* straightLineAction;
    QActionGroup* lineModeGroup;




    QMenu* edgeMenu;
    QMenu* nodeMenu;

    //Sorted vectors for modifying scene
    SortedNodesList x_sorted;
    SortedNodesList y_sorted;

    //Misc
    bool blockWeightUpdates;
    Edge::LabelMode labelMode;
    Edge::LineMode lineMode;
    Node::NodeLabelMode nodeLabelMode;
	bool nodeSelected;
	bool edgeSelected;
	bool firstClick;
	QList<QGraphicsItem*> previousSelection;

    //Current borders
    double left_border;
    double right_border;
    double top_border;
    double bottom_border;

    //Minimum borders
    static const double left_border_min;
    static const double right_border_min;
    static const double top_border_min;
    static const double bottom_border_min;
    static const double extra_space;
};
//! [0]

#endif
