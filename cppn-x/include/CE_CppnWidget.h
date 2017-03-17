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

//Qt includes
//#include <QtGui/QGraphicsView>
#include <QActionGroup>

//Local includes
#include "CE_Defines.h"
#include "CX_ComLabelObject.h"
#include "CX_ComSetPos.h"
#include "CX_ComSetWeight.h"
#include "CX_ComChangeActivation.hpp"
#include "CX_ComAddRemoveObject.h"
#include "CX_ContextMenuGraphicsView.h"
#include "CX_SortedNodesList.h"
#include "CX_Legend.hpp"
#include "CX_Debug.hpp"

class Node;
class Edge;
class Cppn;
//class Window;

//! [0]
class CppnWidget : public ContextMenuGraphicsView
{
    Q_OBJECT

public:

    //Minimum borders
//    static const double left_border_min;
//    static const double right_border_min;
//    static const double top_border_min;
//    static const double bottom_border_min;
    static const double extra_space;
    static const double snapshot_super_resolution;
    static const qreal minimum_zoom_factor;
    static const qreal maximum_zoom_factor;

    CppnWidget(QWidget* widget = 0);

    void deleteCppn();
    void setCppn(QList<Node*> nodes, QList<Edge*> edges);
    Cppn* getCppn(){return cppn;}

    QAction* getAddNodeviewAction(){return addNodeviewAction;}
    QAction* getEdgeNoSignAction(){return _noSignAction;}
    QAction* getEdgeShowSignAction(){return _showSignAction;}
    QAction* getEdgeShowSignIfNoLabelAction(){return _showSignIfNoLabelAction;}
    QAction* getEdgeShowLabelAction(){return _showLabelAction;}
    QAction* getEdgeNoLabelAction(){return _noLabelAction;}
    QAction* getToggleAnnotationsAction(){return _toggleAnnotationsAction;}
    QAction* getCircleAction(){return circleAction;}
    QAction* getLayerAction(){return layerAction;}
    QAction* getONPAction(){return ONPAction;}
    QAction* getNodeLabelAction(){return nodeLabelAction;}
    QAction* getNodeModuleAction(){return nodeModuleAction;}
    QAction* getNodeLabelNoImageAction(){return nodeLabelNoImageAction;}
    QAction* getCurvedLineAction(){return curvedLineAction;}
    QAction* getStraightLineAction(){return straightLineAction;}
    QAction* getIncreaseCurveOffsetAction(){return _increaseCurveOffset;}
    QAction* getDecreaseCurveOffsetAction(){return _decreaseCurveOffset;}
    QAction* getSnapshotAction(){return _snapshotAction;}

    // Zoom actions
    QAction* getZoomInAction(){return _zoomInAction;}
    QAction* getZoomOutAction(){return _zoomOutAction;}
    QAction* getSetZoomAction(){return _setZoomAction;}

    //Find actions
    QAction* getFindUnlabeledEdgeAction(){return _findUnlabeledEdgeAction;}
    QAction* getFindEdgeByIdAction(){return _findEdgeByIdAction;}

    QAction* getWeightEdgeWidthAction(){return _weightEdgeWidthAction;}
    QAction* getFixedEdgeWidthAction(){return _fixedEdgeWidthAction;}

    //Activation functions
    QAction* getSetSinAction(){return _setSin;}
    QAction* getSetCosAction(){return _setCos;}
    QAction* getSetGausAction(){return _setGaus;}
    QAction* getSetSigmoidAction(){return _setSigmoid;}
    QAction* getSetLinAction(){return _setLin;}
    QAction* getSetStepAction(){return _setStep;}
    QAction* getSetUSigmoidAction(){return _setUSigmoid;}
    QAction* getSetUGaussianAction(){return _setUGuas;}
    QAction* getSetUBoundedLinearAction(){return _setUBoundedLinear;}

    QAction* getAddNodeAction(){return _addNode;}
    QAction* getAddNodeOnConnectionAction(){return _addNodeOnConnection;}
    QAction* getAddEdgeAction(){return _addEdge;}

    //Arrange actions
    QAction* getAlignHorizontalAction(){return _alignHorizontal;}
    QAction* getAlignVerticalAction(){return _alignVertical;}
    QAction* getSpaceHorizontalAction(){return _spaceHorizontal;}
    QAction* getSpaceVerticalAction(){return _spaceVertical;}

    QAction* getColorPathAction(){return _colorPathAction;}

    QAction* getScaleLegendAction(){return _scaleLegend;}
//    QAction* getSetFavoriteAction(){return _setFavorite;}

    double getLeftBorder(){return left_border;}
    double getRightBorder(){return right_border;}
    double getTopBorder(){return top_border;}
    double getBottomBorder(){return bottom_border;}

    QActionGroup* getNodeViewGroup(){return nodeViewGroup;}
    QMenu* getEdgeMenu(){return edgeMenu;}
    QMenu* getNodeMenu(){return nodeMenu;}
    bool getNodeSelected(){return nodeSelected;}
    bool getEdgeSelected(){return edgeSelected;}

    Edge* getSelectedEdge();

    size_t getNrOfModules() const;

    void removeNode(Node* node);
    void removeEdge(Edge* edge);

    void setActivationFunction(std::string activationFunction);

    void addLegend(const QList<Label*>& labels);
    void removeLegend();
    Legend* getLegend();

public slots:
	// Zoom functions
    void zoomIn();
    void zoomOut();
    void setZoom();

    //Experimental functions
    void increaseCurveOffset();
    void decreaseCurveOffset();

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

    void setEdgeShowSign();
    void setEdgeShowSignIfNoLabel();
    void setEdgeNoSign();

    void setEdgeShowLabel();
    void setEdgeNoLabel();

    void setEdgeWeightWidth();
    void setEdgeFixedWidth();

    void setShowAnnotations();
    void setHideAnnotations();
    void toggleAnnotations();

    void setLabelView();
    void setSignView();
    void setLabelAndSignView();

    void setNodeLabelView();
    void setNodeModuleView();
    void setNodeLabelNoImageView();

//    void setFavourite();

    //Search functions
    void findUnlabeledEdge();
    void findEdgeById();

    //Update functions
    void updateSelection();
//    void updateAll();
    void updatePreviousPositions();
    void rebuildPhenotype();

    void itemMoved(MovableObject* object);

    void saveImage();

    void deleteObjects();

    void deselectItems();

    void flash(bool flashOn);
//    void ContextMenuEvent(SelectableObject* object, bool begin);

    //Experimental
    void snapShot();
    void scaleLegend();
    void colorPaths();

    //Change activation function functions
    void setSin();
    void setCos();
    void setSigmoid();
    void setGaus();
    void setLin();
    void setStep();
    void setUGuas();
    void setUSigmoid();
    void setUBoundedLinear();

    //Add node functions
    void addNode();
    void addNodeOnConnection();
    void addEdge();

    //Arrange functions
    void alignHorizontal();
    void alignVertical();
    void spaceHorizontal();
    void spaceVertical();


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

    QList<Node*> _getSelectedNodes();

    Cppn* cppn;

    //Actions
    QAction* addNodeviewAction;

    QAction* _zoomInAction;
    QAction* _zoomOutAction;
    QAction* _setZoomAction;

//    QAction *labelOnlyAction;
//    QAction *signOnlyAction;
//    QAction *labelAndSignAction;
//    QActionGroup* viewGroup;

    QAction* _noLabelAction;
    QAction* _showLabelAction;
    QActionGroup* _edgeShowLabelGroup;

    QAction* _showSignAction;
    QAction* _showSignIfNoLabelAction;
    QAction* _noSignAction;
    QActionGroup* _edgeShowSignGroup;

    QAction* _weightEdgeWidthAction;
    QAction* _fixedEdgeWidthAction;
    QActionGroup* _edgeWidthGroup;

    QAction* _toggleAnnotationsAction;

    //Positioning actions
    QAction* layerAction;
    QAction* ONPAction;
    QAction* circleAction;


    QAction* nodeLabelAction;
    QAction* nodeModuleAction;
    QAction* nodeLabelNoImageAction;
    QActionGroup* nodeViewGroup;

    QAction* curvedLineAction;
    QAction* straightLineAction;
    QActionGroup* lineModeGroup;

    QAction* _increaseCurveOffset;
    QAction* _decreaseCurveOffset;
    QAction* _snapshotAction;
    QAction* _colorPathAction;

    //Find actions
    QAction* _findUnlabeledEdgeAction;
    QAction* _findEdgeByIdAction;

    //Edit node actions
    QAction* _setSin;
    QAction* _setCos;
    QAction* _setGaus;
    QAction* _setSigmoid;
    QAction* _setLin;
    QAction* _setStep;
    QAction* _setUSigmoid;
    QAction* _setUGuas;
    QAction* _setUBoundedLinear;

    //Add node actions
    QAction* _addNode;
    QAction* _addNodeOnConnection;
    QAction* _addEdge;

    //Arrange functions
    QAction* _alignHorizontal;
    QAction* _alignVertical;
    QAction* _spaceHorizontal;
    QAction* _spaceVertical;


    QAction* _scaleLegend;
    //
//    QAction* _setFavorite;

    QMenu* edgeMenu;
    QMenu* nodeMenu;

    //Sorted vectors for modifying scene
    SortedNodesList left_sorted;
    SortedNodesList right_sorted;
    SortedNodesList top_sorted;
    SortedNodesList bottom_sorted;

    //Misc
    bool blockWeightUpdates;
    Edge::LabelMode labelMode;
    Edge::EdgeSignMode _signMode;
    Edge::LineMode lineMode;
    Edge::EdgeLineWidthMode _lineWidthMode;
    Edge::EdgeLAnnotationMode _annotationMode;
    Node::NodeLabelMode nodeLabelMode;
	bool nodeSelected;
	bool edgeSelected;
	bool firstClick;

	Node* _firstNodeSelected;

	Legend* _legend;
//	QList<QGraphicsItem*> previousSelection;
//	size_t _current_favourite_index;

    //Current borders
    double left_border;
    double right_border;
    double top_border;
    double bottom_border;
};
//! [0]

#endif
