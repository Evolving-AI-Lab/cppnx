/*
 * CX_CsvParser.cpp
 *
 *  Created on: Mar 29, 2017
 *      Author: joost
 */

// Local includes
#include "CX_CsvParser.hpp"
#include "CX_Debug.hpp"
#include "CE_FinalNodeView.h"

CsvParser::CsvParser(QString fileName){
    dbg::trace trace("parser", DBG_HERE);
    _qFileName = fileName;
    QFileInfo fileInfo(_qFileName);

    _fileInformation = new FileInformation(true);
    _cppnInformation = new CppnInformation();

    if(!fileInfo.exists()){
        throw CeParseException(QString("File does not exist: " + _qFileName).toStdString());
    }
}

CsvParser::~CsvParser(){
    dbg::trace trace("parser", DBG_HERE);
    foreach(Label* label, _labels){
        label->unregisterObject();
    }
    _labels.clear();

    foreach(Node* node, _nodes){
        delete node;
    }
    _nodes.clear();

    foreach(Edge* edge, _edges){
        delete edge;
    }
    _edges.clear();

    foreach(NodeView* nodeview, _nodeviews){
        delete nodeview;
    }
    _nodeviews.clear();

    if(_fileInformation) delete _fileInformation;
    _fileInformation = 0;

    if(_cppnInformation) delete _cppnInformation;
    _cppnInformation = 0;
}

int CsvParser::_get_nb_layer_inputs(layer_t& layer, bool inputs_as_columns){
	if(layer.empty()){
		return 0;
	} else if(inputs_as_columns){
		return layer.length();
	} else {
		return layer.first().length();
	}
}

int CsvParser::_get_nb_layer_outputs(layer_t& layer, bool inputs_as_columns){
	if(layer.empty()){
		return 0;
	} else if(inputs_as_columns){
		return layer.first().length();
	} else {
		return layer.length();
	}
}

void CsvParser::parse(int generation){
	Q_UNUSED(generation);
    dbg::trace trace("parser", DBG_HERE);
    dbg::out(dbg::info, "parser") << "Parsing file: " << _qFileName.toStdString() << std::endl;

    // Parameters
	std::string af_input=XML_LINEAR;
	std::string af_hidden=XML_TANH;
	std::string af_output=XML_U_SIGMOID;
	std::string color_space="unknown";
	bool inputs_as_columns = true;
	QList<std::string> input_labels;
	QList<std::string> output_labels;
	qreal x_min = -1;
	qreal x_max = 1;
	qreal y_min = -1;
	qreal y_max = 1;
	int x_res = IMAGE_WIDTH;
	int y_res = IMAGE_HEIGHT;

	// Keep track of which parameters were set
	bool af_input_set = false;
	bool af_hidden_set = false;
	bool af_output_set = false;
	bool color_space_set = false;
	bool inputs_as_set = false;
	bool input_labels_set = false;
	bool output_labels_set = false;
	bool x_min_set = false;
	bool x_max_set = false;
	bool y_min_set = false;
	bool y_max_set = false;
	bool x_res_set = false;
	bool y_res_set = false;

	// Other variables
    int lastLineLength = 0;
    QList<layer_t> layers;
    bool unparsedLine = false;

    // Open file
	QFile inputFile(_qFileName);
	if (inputFile.open(QIODevice::ReadOnly)){
	   QTextStream in(&inputFile);
	   QString line;

	   // Read parameters from the CSV
	   while (!in.atEnd()){
		   line = in.readLine();
		   if(line.startsWith('#')){
			   // Eat the hash tag
			   line.remove('#');
			   // Normalize whitespace
			   line = line.simplified();
			   // Split string based on space
			   QStringList splitLine = line.split(' ');
			   if(splitLine.size() >= 1){
				   QString property = splitLine[0];
				   property = property.trimmed();
				   if(property.endsWith(":")){
					   line.remove(':');
				   }
				   if(property == "activation_function_hidden" || property == "af_hidden"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(af_hidden_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   af_hidden = splitLine[1].toStdString();
					   af_hidden_set = true;
				   } else if(property == "activation_function_input" || property == "af_input"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(af_input_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   af_input = splitLine[1].toStdString();
					   af_input_set = true;
				   } else if(property == "activation_function_output" || property == "af_output"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(af_output_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   af_output = splitLine[1].toStdString();
					   af_output_set = true;
				   } else if(property == "color_space" || property == "cs"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(color_space_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   if(output_labels_set){
						   throw CeParseException((QString("Outputs determine the color space, so color space should not be set when outputs is defined: ") +property).toStdString());
					   }
					   color_space = splitLine[1].toStdString();
					   color_space_set = true;
				   } else if(property == "x_min"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(x_min_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   x_min = splitLine[1].toDouble();
					   x_min_set = true;
				   } else if(property == "x_max"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(x_max_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   x_max = splitLine[1].toDouble();
					   x_max_set = true;
				   } else if(property == "y_min"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(y_min_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   y_min = splitLine[1].toDouble();
					   y_min_set = true;
				   } else if(property == "y_max"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(y_max_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   y_max = splitLine[1].toDouble();
					   y_max_set = true;
				   } else if(property == "x_res"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(x_res_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   x_res = splitLine[1].toDouble();
					   x_res_set = true;
				   } else if(property == "y_res"){
					   if(splitLine.length() < 2){
						   throw CeParseException((QString("Missing value for property: ") +property).toStdString());
					   }
					   if(y_res_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   y_res = splitLine[1].toDouble();
					   y_res_set = true;
				   } else if(property == "inputs"){
					   if(input_labels_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   for(int i=1; i<splitLine.size(); ++i){
						   input_labels.append(splitLine[i].toStdString());
					   }
					   input_labels_set = true;
				   } else if(property == "outputs"){
					   if(output_labels_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   if(color_space_set){
						   	   throw CeParseException((QString("Outputs determine the color space, so color space should not be set when outputs is defined: ") +property).toStdString());
					   }
					   for(int i=1; i<splitLine.size(); ++i){
						   output_labels.append(splitLine[i].toStdString());
					   }
					   output_labels_set = true;
				   } else if(property == "inputs_as_columns"){
					   if(inputs_as_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   inputs_as_columns = true;
					   inputs_as_set = true;
				   } else if(property == "inputs_as_rows"){
					   if(inputs_as_set){
						   throw CeParseException((QString("Property set twice: ") +property).toStdString());
					   }
					   inputs_as_columns = false;
					   inputs_as_set = true;
				   } else{
					   throw CeParseException((QString("Found unknown property: ") + property).toStdString());
				   }
			   }
		   } else {
			   unparsedLine = true;
			   break;
		   }
	   }

	   // Read the matrices from the csv file
	   while (!in.atEnd()){
		  dbg::out(dbg::info, "parser") << "Reading line" << std::endl;
		  if(!unparsedLine){
			  line = in.readLine();
		  } else {
			  unparsedLine = false;
		  }
	      dbg::out(dbg::info, "parser") << "Read line: " << line.toStdString() << std::endl;

	      // Parse line into numbers
	      QStringList splitLine = line.split(',');
	      line_t new_line;
	      foreach(QString line, splitLine){
	    	  bool result;
	    	  qreal parsedNumber = line.toDouble(&result);
	    	  if(!result){
	    		  //std::cout << "Error: while parsing a double" << std::endl;
	    	  } else {
	    		  dbg::out(dbg::info, "parser") << "Read number: " << parsedNumber << std::endl;
	    		  new_line.append(parsedNumber);
	    	  }
	      }

	      // If necessary, add a new layer
	      if(new_line.length() == 0){
	    	  lastLineLength = 0;
	      } else if(new_line.length() != lastLineLength){
	    	  dbg::out(dbg::info, "parser") << "Adding new layer" << std::endl;
	    	  layers.append(layer_t());
	    	  lastLineLength = new_line.length();
	      }

	      // If the line we just read contained any numbers, add them to the most recent layer
	      if(new_line.length() > 0){
	    	  dbg::out(dbg::info, "parser") << "Adding new line in layer" << std::endl;
	    	  layers.back().append(new_line);
	      }
	   }
	   inputFile.close();
	} else{
		throw CeParseException("Unable to open file!");
	}
	if(layers.empty()){
		throw CeParseException(QString("Error while parsing file!").toStdString());
	}

	// Fix input_labels
	if(!input_labels.contains(INPUT_X)){
		input_labels.append(INPUT_X);
	}
	if(!input_labels.contains(INPUT_Y)){
		input_labels.append(INPUT_Y);
	}
	if(!input_labels.contains(INPUT_D)){
		input_labels.append(INPUT_D);
	}
	if(!input_labels.contains(INPUT_BIAS)){
		input_labels.append(INPUT_BIAS);
	}

	// Fix output_labels
	if(output_labels.empty()){
		if(color_space == "unknown"){
			if(layers.back().size() >= 3){
				color_space="rgb";
			} else {
				color_space="grey";
			}
		}
		if(color_space=="rgb"){
			output_labels.append(OUTPUT_RED);
			output_labels.append(OUTPUT_GREEN);
			output_labels.append(OUTPUT_BLUE);
		} else if(color_space=="hsv"){
			output_labels.append(OUTPUT_HUE);
			output_labels.append(OUTPUT_BRIGTHNESS);
			output_labels.append(OUTPUT_SATURATION);
		} else if(color_space=="grey"){
			output_labels.append(OUTPUT_INK);
		}
	}

	// Set CPPN information
	_cppnInformation->min_x = x_min;
	_cppnInformation->max_x = x_max;
	_cppnInformation->min_y = y_min;
	_cppnInformation->max_y = y_max;
	_cppnInformation->x_res = x_res;
	_cppnInformation->y_res = y_res;

	// Add input nodes
	dbg::out(dbg::info, "parser") << "Adding input nodes" << std::endl;
	QList<QList<Node*> > nodes;
	nodes.append(QList<Node*>());
	int temp = std::max(_get_nb_layer_inputs(layers.first(), inputs_as_columns), int(input_labels.size()));
	for(int i=0; i<temp; ++i){
		std::string label = "";
		if(i < input_labels.size()){
			label = input_labels[i];
		}
		dbg::out(dbg::info, "parser") << "Input added: " << label << std::endl;
		nodes.back().append(new Node("", "", XML_TYPE_INPUT, af_input, label));
		_nodes.append(nodes.back().back());
	}
	dbg::out(dbg::info, "parser") << "Number of inputs: " << nodes.back().size() << std::endl;

	// Add hidden and output nodes
	dbg::out(dbg::info, "parser") << "Adding hidden and output nodes" << std::endl;
	int nb_outputs = 0;
	for(int i=0; i<layers.size(); ++i){
		layer_t& layer = layers[i];
		int nbOfOutputs = _get_nb_layer_outputs(layer, inputs_as_columns);
		int nbOfInputs = _get_nb_layer_inputs(layer, inputs_as_columns);
		dbg::out(dbg::info, "parser") << "Layer: " << i << std::endl;
		dbg::out(dbg::info, "parser") << "Nb inputs: " << nbOfInputs << std::endl;
		dbg::out(dbg::info, "parser") << "Nb outputs: " << nbOfOutputs << std::endl;
		if(layer.empty()) continue;
		nodes.append(QList<Node*>());
		//int nbOfInputs = layer.length();

		if((i+1) == layers.size()){
			if(nbOfOutputs == 1){
				dbg::out(dbg::info, "parser") << "Output added: " << OUTPUT_INK << std::endl;
				nb_outputs+=1;
				nodes.back().append(new Node("", "", XML_TYPE_OUTPUT, af_output, OUTPUT_INK));
				_nodes.append(nodes.back().back());
			} else {
				for(int i=0; i<nbOfOutputs; ++i){
					nb_outputs+=1;
					std::string label = "";
					if(i < output_labels.size()){
						label = output_labels[i];
					}
					dbg::out(dbg::info, "parser") << "Output added: " << label << std::endl;
					nodes.back().append(new Node("", "", XML_TYPE_OUTPUT, af_output, label));
					_nodes.append(nodes.back().back());
				}
			}

		} else{
			for(int i=0; i<nbOfOutputs; ++i){
				dbg::out(dbg::info, "parser") << "Node added" << std::endl;
				nodes.back().append(new Node("", "", XML_TYPE_HIDDEN, af_hidden));
				_nodes.append(nodes.back().back());
			}
		}
	}

	// Add edges
	dbg::out(dbg::info, "parser") << "Adding edges" << std::endl;
	for(int i=0; i<layers.size(); ++i){
		layer_t& layer = layers[i];
		dbg::out(dbg::info, "parser") << "Rows in layer: " << layer.size() << std::endl;
		for(int row=0; row<layer.size(); ++row){
			dbg::out(dbg::info, "parser") << "Values in row: " << layer[row].size() << std::endl;
			for(int column=0; column<layer[row].size(); ++column){
				dbg::out(dbg::info, "parser") << "Adding edge: " << i << ", " << row << ", " << column << std::endl;
				if(!inputs_as_columns){
					dbg::out(dbg::info, "parser") << "Adding edge from layer: " << i <<
							" node: " << column <<
							" to layer: " << i+1 <<
							" node: " << row << std::endl;
					_edges.append(new Edge("", "", nodes[i][column], nodes[i+1][row], layers[i][row][column], layers[i][row][column]));
				} else{
					dbg::out(dbg::info, "parser") << "Adding edge from layer: " << i <<
							" node: " << column <<
							" to layer: " << i+1 <<
							" node: " << row << std::endl;
					_edges.append(new Edge("", "", nodes[i][row], nodes[i+1][column], layers[i][row][column], layers[i][row][column]));
				}
			}
		}
	}

	// Create final node view for the network
	Node* value = 0;
	Node* hue = 0;
	Node* saturation = 0;
	Node* r = 0;
	Node* g = 0;
	Node* b = 0;
	for(int i=0; i<nodes.back().size(); ++i){
		if(nodes.back()[i]->getXmlLabel() == OUTPUT_BRIGTHNESS || nodes.back()[i]->getXmlLabel() == OUTPUT_INK){
			value = nodes.back()[i];
		} else if(nodes.back()[i]->getXmlLabel() == OUTPUT_HUE){
			hue = nodes.back()[i];
		} else if(nodes.back()[i]->getXmlLabel() == OUTPUT_SATURATION){
			saturation = nodes.back()[i];
		} else if(nodes.back()[i]->getXmlLabel() == OUTPUT_RED){
			r = nodes.back()[i];
		} else if(nodes.back()[i]->getXmlLabel() == OUTPUT_GREEN){
			g = nodes.back()[i];
		} else if(nodes.back()[i]->getXmlLabel() == OUTPUT_BLUE){
			b = nodes.back()[i];
		}
	}
	if((value||hue||saturation) && (r||g||b)){
		throw CeParseException(QString("Found both hsv and rgb output nodes. These nodes can't be combined.").toStdString());
	}

	// Add the appropriate node view
	FinalNodeView* finalNodeview = new FinalNodeView();
	if(value) finalNodeview->setValueNode(value);
	if(hue) finalNodeview->setHueNode(hue);
	if(saturation) finalNodeview->setSaturationNode(saturation);
	if(r) finalNodeview->setRNode(r);
	if(g) finalNodeview->setGNode(g);
	if(b) finalNodeview->setBNode(b);
	_nodeviews.append(finalNodeview);

}

