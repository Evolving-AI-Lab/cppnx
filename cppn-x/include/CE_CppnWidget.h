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
#include <JGTL_LocatedException.h>
#include <QtGui/QGraphicsView>
#include <map>
#include "CE_Edge.h"
#include "CE_Node.h"
#include "CE_Defines.h"
#include "CE_CppnParser.h"
#include "CE_Cppn.h"
#include "CE_CppnWriter.h"
#include "CE_LabelWidget.h"
#include "CE_Window.h"



class Node;
class Edge;
class Cppn;
class Window;

//! [0]
class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget(Window *window, QWidget *parent = 0);

    void itemMoved(Node * node);
    bool load(std::string filename);
    bool save(std::string filename);
//    void edgeSelected(int id, Edge* selectedEdge);
    Window* getWindow();
    Node* getSelectedNode(){
    	return selectedNode;
    }

    Edge* getSelectedEdge(){
    	return selectedEdge;
    }

    int getGeneration(int min, int max);
    void warning(std::string message);
    void updateAll();

public slots:
//    void shuffle();
    void zoomIn();
    void zoomOut();
    void setValue(int value);
    void setValueF(double value);
//    void selectEdge();
    void resetWeight();
    void resetAllWeights();
    void scanWeight();
    void positionNodesLayers();
    void positionNodesCircle();


    void addNode(Node* node);

//    int minimumHeight() const{
//    	std::cout << "min heigth" << std::endl;
//    	return 200;
//    }
//
//    QSize minimumSize() const;
//    QSize minimumSizeHint() const;
//
//    int heightForWidth(int w) const{
//    	std::cout << "hfw Used" << std::endl;
//    	return 0;
//    }

signals:
void sliderValueChanged(int newValue);
void sliderValueChangedF(double newValue);

protected:
//    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
    void wheelEvent(QWheelEvent *event);
//    void drawBackground(QPainter *painter, const QRectF &rect);

    void scaleView(qreal scaleFactor);
//    void resizeEvent(QResizeEvent * event){
//    	std::cout << "New size: " << size().height() << std::endl;
//    }
//    void updateNodes();
//    void positionNodes();



private:
    int timerId;
    Cppn* cppn;
    //shared_ptr<NEAT::FastNetwork<double> > cppn_phen;
    //std::vector<Node*> nodes;

    //static const int width = 100;
    //static const int height = 100;

    //int edgeId;
    int sliderValue;
    Edge* selectedEdge;
    Node* selectedNode;
    Window *par_window;

    std::vector<Node*> x_sorted;
    std::vector<Node*> y_sorted;

    double left_border;
    double right_border;
    double top_border;
    double bottom_border;

    static const double left_border_min;
    static const double right_border_min;
    static const double top_border_min;
    static const double bottom_border_min;
    static const double extra_space;
};
//! [0]

#endif
