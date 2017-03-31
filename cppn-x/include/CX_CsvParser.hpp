/*
 * CX_CsvParser.hpp
 *
 *  Created on: Mar 29, 2017
 *      Author: joost
 */

#ifndef CPPN_X_INCLUDE_CX_CSVPARSER_HPP_
#define CPPN_X_INCLUDE_CX_CSVPARSER_HPP_

#include "CE_Parser.hpp"
#include "CE_Label.h"
#include "CE_Node.h"
#include "CE_Edge.h"
#include "CE_NodeView.h"
#include "CX_FileInformation.h"
#include "CX_CppnInformation.hpp"

class CsvParser{

public:
    typedef QList<qreal> line_t;
    typedef QList<line_t> layer_t;

	CsvParser(QString fileName);
	virtual ~CsvParser();
	void parse(int generation = -1);


	/**
	 * Returns a list of pointers to labels.
	 */
	QList<Label*> getLabels(){
		return _labels;
	}

    /**
     * Returns a list of pointers to nodes.
     *
     * Does not transfer ownership, as soon as the parser gets deleted,
     * all pointers will be invalidated.
     */
	QList<Node*> getNodes(){
		return _nodes;
	}

    /**
     * Returns a list of pointers to edges.
     *
     * Does not transfer ownership, as soon as the parser gets deleted,
     * all pointers will be invalidated.
     */
	QList<Edge*> getEdges(){
		return _edges;
	}

    /**
     * Returns a list of pointers to node views.
     *
     * Does not transfer ownership, as soon as the parser gets deleted,
     * all pointers will be invalidated.
     */
	QList<NodeView*> getNodeviews(){
		return _nodeviews;
	}

    /**
     * Returns pointer to the file information.
     *
     * Does not transfer ownership, as soon as the parser gets deleted,
     * all pointers will be invalidated.
     */
	FileInformation* getFileInformation(){
		return _fileInformation;
	}

    /**
     * Returns pointer to the cppn information.
     *
     * Does not transfer ownership, as soon as the parser gets deleted,
     * all pointers will be invalidated.
     */
	CppnInformation* getCppnInformation(){
		return _cppnInformation;
	}

    /**
     * Returns a list of pointers to nodes, and clears the list.
     *
     * Transfers ownership of the pointers to the caller,
     * all objects retrieved this way will not be destroyed by the parser.
     */
    QList<Node*> takeNodes(){
        QList<Node*> temp = _nodes;
        _nodes.clear();
        return temp;
    }

    /**
     * Returns a list of pointers to edges, and clears the list.
     *
     * Transfers ownership of the pointers to the caller,
     * all objects retrieved this way will not be destroyed by the parser.
     */
    QList<Edge*> takeEdges(){
        QList<Edge*> temp = _edges;
        _edges.clear();
        return temp;
    }

    /**
     * Returns a list of pointers to nodeviews, and clears the list.
     *
     * Transfers ownership of the pointers to the caller,
     * all objects retrieved this way will not be destroyed by the parser.
     */
    QList<NodeView*> takeNodeviews(){
        QList<NodeView*> temp = _nodeviews;
        _nodeviews.clear();
        return temp;
    }

    /**
     * Returns a pointer to file information and sets the pointer to 0.
     *
     * Transfers ownership of the pointer to the caller,
     * the object retrieved this way will not be destroyed by the parser.
     */
    FileInformation* takeFileInformation(){
        FileInformation* temp = _fileInformation;
        _fileInformation = 0;
        return temp;
    }

    /**
     * Returns a pointer to the cppn information and sets the pointer to 0.
     *
     * Transfers ownership of the pointer to the caller,
     * the object retrieved this way will not be destroyed by the parser.
     */
    CppnInformation* takeCppnInformation(){
    	CppnInformation* temp = _cppnInformation;
        _cppnInformation = 0;
        return temp;
    }

protected:
    int _get_nb_layer_inputs(layer_t& layer, bool inputs_as_columns);
    int _get_nb_layer_outputs(layer_t& layer, bool inputs_as_columns);

	FileInformation* _fileInformation;
	CppnInformation* _cppnInformation;
	QList<Label*> _labels;
	QList<Node*> _nodes;
	QList<NodeView*> _nodeviews;
	QList<Edge*> _edges;

	//std::istream* _myfile;
	QString _qFileName;
};

#endif /* CPPN_X_INCLUDE_CX_CSVPARSER_HPP_ */
