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

#ifndef NODE_H
#define NODE_H

#include <QtGui>
#include <QGraphicsItem>
#include <QList>
#include <QPointer>

#include "CE_Defines.h"
#include "CE_Edge.h"
#include "CE_LabelableObject.h"




class Label;
class Edge;
class NodeView;
class FinalNodeView;
class Cppn;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE


class LabelModeException : public std::exception
{
    std::string text;

public:

    LabelModeException(const std::string &_reason){
        text = _reason;
    }

    ~LabelModeException() throw (){

    }

    virtual const char* what() const throw(){
        return text.c_str();
    }
};


//! [0]
class Node : public LabelableObject
{
	Q_OBJECT
public:
	static size_t nodes_in_memory;

    enum NodeLabelMode {labels, modules, labelsNoImage};
    enum NodeImageMode {showImage, noImage};
    enum NodeType      {input, hidden, output};

    /**
     * Creates a new node based on explicit information.
     *
     * @param branch                 The branch of the node.
     *                               A branch is an identifier associated with one Picbreeder session,
     *                               a new 'branch' starts whenever a user picks an image (evolved or from scratch)
     *                               and starts evolving that image. A branch stops as soon as the user saves or discards the image.
     *                               Each node is tagged according to the branch in which it was added, meaning that a single network
     *                               can have nodes from multiple branches if the network was evolved by multiple users.
     *                               When creating a network from scratch you can chose an arbitrary branch but,
     *                               if you were to ever use this tool to compare your network with other networks,
     *                               this tool may mistakenly classify different nodes as being the same.
     * @param id                     The id of this node.
     *                               Is used to distinguish nodes when saving or loading files, and should be unique for each
     *                               node and each edge.
     * @param type                   The type of this node as a string.
     *                               Types available in CE_Defines.h are:
     *                               - XML_TYPE_INPUT
     *                               - XML_TYPE_OUTPUT
     *                               - XML_TYPE_HIDDEN
     * @param activationFunction_str The activation function as a string:
     *                               Activation function, available in CE_Defines.h are:
     *                               - XML_GAUSSIAN
     *                               - XML_LINEAR
     *                               - XML_SIN
     *                               - XML_COS
     *                               - XML_SIGMOID
     * @param xml_label              The xml label determines the type of input or output of this node,
     *                               if this node is an input or output.
     *                               The xml field is called 'label' which is why this parameter is called the xml_label.
     *                               Output type, available in CE_Defines.h are:
     *                               - OUTPUT_INK         The only output for grey scale networks
     *                               - OUTPUT_BRIGTHNESS  The brightness for color networks
     *                               - OUTPUT_SATURATION  The saturation for color networks
     *                               - OUTPUT_HUE         The hue for color networks
     *                               - INPUT_X            The x input
     *                               - INPUT_Y            The y input
     *                               - INPUT_D            The distance from the center input
     *                               - INPUT_BIAS         The bias input (always 1)
     * @param affinity               The affinity of this node in color networks.
     *                               Determines whether this node will change if the user decided to only evolve color.
     *                               This value is not used by cppn-x, and can safely be omitted.
     * @param bias                   The bias of this node.
     *                               While present in the xml files, the bias is never set in the Picbreeder networks,
     *                               and as a result the bias is not used by the cppn network defined here
     *                               (setting it here does NOT change the function of the node)
     *                               A sensible default value is thus: 0.0.
     * @param label                  The label associated with this node.
     *                               Set to null if this node is unlabeled.
     * @param note                   A custom note that can be added to a node object.
     *                               This value is written to the xml file, but it is currently not displayed
     *                               anywhere within the program, nor can it be edited.
     */
    Node(
    		std::string branch = "",
    		std::string id = "",
    		std::string type = "",
    		std::string activationFunction_str=XML_SIGMOID,
    		std::string xml_label = "",
    		std::string affinity = "",
    		std::string bias = "",
    		Label* label = 0,
    		std::string note = ""
    );

    Node(std::iostream &stream, std::map<std::string, Label*> labelMap);
    ~Node();

    void init();
    void reinitImage();

    void addIncommingEdge(Edge *edge);
    void addOutgoingEdge(Edge *edge);

    void removeIncommingEdge(Edge *edge);
    void removeOutgoingEdge(Edge *edge);

    const QList<Edge *>& incomingEdges() const;
    const QList<Edge *>& outgoingEdges() const;

    enum { Type = UserType + NODE_TYPE };
    int type() const { return Type; }

//    void calculateForces();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPixels(QImage* pixels_);
    void setPixel(int x, int y, char r, char g, char b);
    void setPixel(size_t index, char r, char g, char b);
    void setPixel(size_t index, char grey);
    void setPixel(size_t index, const double& value);

    void updateAll();

   // std::string getName(){return name;}
    std::string getType(){return nodetype;}

    void setActivationFunction(const std::string& xmlActivationFunction);

    void setActivationFunction(ActivationFunctionPt _activationFunction){
    	activationFunction =_activationFunction;
    }

    ActivationFunctionPt getActivationFunction(){
    	return activationFunction;
    }

    void setCppn(Cppn* cppn){_cppn = cppn;}
    Cppn* getCppn(){return _cppn;}

    void setIndex(size_t _index){
    	index =_index;
    }

    size_t getIndex(){
    	return index;
    }

    std::string getXmlLabel(){
    	return xml_label;
    }

    std::string getBranch(){
    	return branch;
    }

    std::string getId(){
    	return id;
    }

    std::string getName() const{
    	return branch + "_" + id;
    }

    std::string getXmlActivationFunction(){
    	return activationFunction_str;
    }

    void setLabelMode(NodeLabelMode* _labelMode){
    	labelMode = _labelMode;
    }

    QImage* getImage(){
    	return pixels;
    }


    std::string getAffinity(){return affinity;};
    std::string getBias(){return bias;};

    size_t getModule(){return module;};

    void redraw();



    void setDepth(int _depth){
    	depth =_depth;
    }

    int getDepth(){
    	return depth;
    }

    void finishImage(){
    	update();
    }

    void setModule(size_t _module){
    	module =_module;
    }

    void emitRemoved(){
    	emit removed();
    }

    void emitAdded(){
    	emit added();
    }

    NodeType getNodeType(){
        return _nodeType;
    }

    void setNodeType(NodeType nodeType){
        _nodeType = nodeType;
    }

    void setCustomColor(QColor color){
        _customColor = color;
        _useCustomColor = QBool(true);
        update();
    }

    static const int node_width = 40;
    static const int node_height = 40;
    static const int half_width = 20;
    static const int half_height = 20;
    static const int footerBarSize = 10;
    static const int headerBarSize = 10;



signals:
	void updateRequest(Node*);
	void imageChanged();
	void removed();
	void added();
	void imageResized();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

//    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    
private:
    QList<Edge *> incomingEdgeList;
    QList<Edge *> outgoingEdgeList;

    QImage* pixels;

    std::string branch;
    std::string id;
    std::string nodetype;
    std::string activationFunction_short;
    std::string activationFunction_str;
    std::string xml_label;
    std::string affinity;
    std::string bias;

    ActivationFunctionPt activationFunction;

    size_t index;

    int depth;
    size_t module;

    NodeType _nodeType;

    NodeLabelMode* labelMode;

    QBool _useCustomColor;
    QColor _customColor;

    Cppn* _cppn;
};

std::ostream& operator<< (std::ostream& os, const Node& obj);

#endif
