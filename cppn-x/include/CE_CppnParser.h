/*
 * cppnParser.h
 *
 *  Created on: May 23, 2013
 *      Author: joost
 */

//#include <exception>

#ifndef CPPNPARSER_H_
#define CPPNPARSER_H_

#include "CE_Defines.h"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <JGTL_LocatedException.h>
#include <map>
//#include <errno.h>
//#include <boost/exception/all.hpp>

//typedef boost::error_info<struct tag_errno,std::string> err_msg; //(1)

//class ParseException: public boost::exception, public std::exception { }; //(2)

typedef std::map<std::string, std::string> io_map_t;
//typedef std::map<std::string, std::string> io_map_t;

class CppnParser
{
public:
	CppnParser(){

	}


	static shared_ptr<NEAT::GeneticIndividual> parse(std::string fileName, io_map_t& io_map){
		static const boost::regex first_line("<\?xml.*\?>");
		static const boost::regex ionode("\\s*<node label=\"(.*?)\" type=\"(.*?)\">");
		static const boost::regex node("\\s*<node type=\"(.*?)\">");
		static const boost::regex link("\\s*<link>");
		static const boost::regex marking("\\s*<marking branch=\"(\\d*)\" id=\"(\\d*)\"/>");
		static const boost::regex activation("\\s*<activation>(.*)</activation>");
		static const boost::regex source("\\s*<source branch=\"(\\d*)\" id=\"(\\d*)\"/>");
		static const boost::regex target("\\s*<target branch=\"(\\d*)\" id=\"(\\d*)\"/>");
		static const boost::regex weight("\\s*<weight>(.*)</weight>");
		boost::smatch m;

		bool parsing_node=false;
		bool parsing_link=false;
		bool done_parsing=false;
		bool first_link = true;

		bool is_first_line = true;

		std::string label = "";
		std::string xml_type = "";
		std::string activation_function = "";
		std::string id = "";
		std::string id_branch = "";
		std::string source_id = "";
		std::string source_branch = "";
		std::string target_id = "";
		std::string target_branch = "";

		std::string type = "";
		double drawing_posistion = DRAW_POS_ZERO;
		ActivationFunction activation_function_enum = ACTIVATION_FUNCTION_SIGMOID;
		double weight_f=0;


		std::string line;
		std::ifstream myfile (fileName.c_str());
		//myfile.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

		NEAT::GeneticIndividual* cppn = NULL;
		std::vector<NEAT::GeneticNodeGene> nodeVector;
		std::vector<NEAT::GeneticLinkGene> linkVector;

		if (myfile.is_open())
		{
			while ( myfile.good() )
			{
				getline (myfile,line);
//				std::cout << line << std::endl;

				if(is_first_line){
					if(!regex_search(line, m, first_line)){
						throw JGTL::LocatedException("File does not appear to be an XML file");
					}
					is_first_line = false;
				}

				if(regex_search(line, m, node)){
					//cout << "node" << endl;
					//cout << "label: " << m[1].str() << endl;
					xml_type = m[1].str();
					parsing_node = true;
				} else if(regex_search(line, m, ionode)){
					//cout << "node" << endl;
					//cout << "label: " << m[1].str() << endl;
					label = m[1].str();
					xml_type = m[2].str();
					parsing_node = true;
				} else if(regex_search(line, m, link)){
//					std::cout << "link" << std::endl;
					parsing_link = true;
				} else if(regex_search(line, m, marking)){
//					std::cout << "marking" << std::endl;
//					std::cout << "branch: " << m[1].str() << std::endl;
//					std::cout << "id: " << m[2].str() << std::endl;
					id_branch = m[1].str();
					id = m[1].str()+"_"+m[2].str();

				} else if(regex_search(line, m, activation)){
					//cout << "activation" << endl;
					//cout << "function: " << m[1].str() << endl;
					activation_function = m[1].str();
					done_parsing = true;
				} else if(regex_search(line, m, source)){
//					std::cout << "source" << std::endl;
//					std::cout << "branch: " << m[1].str() << std::endl;
//					std::cout << "id: " << m[2].str() << std::endl;
					source_branch = m[1].str();
					source_id = m[1].str()+"_"+m[2].str();
				} else if(regex_search(line, m, target)){
//					std::cout << "target" << std::endl;
//					std::cout << "branch: " << m[1].str() << std::endl;
//					std::cout << "id: " << m[2].str() << std::endl;
					target_branch = m[1].str();
					target_id = m[1].str()+"_"+m[2].str();
				} else if(regex_search(line, m, weight)){
//					std::cout << "weight" << std::endl;
//					std::cout << "weight: " << m[1].str() << std::endl;
					weight_f= lexical_cast<double>(m[1].str());
					done_parsing = true;
				}
				if(done_parsing){
					if(parsing_node){
//						std::cout << "node" << std::endl;
//						std::cout << "type: " << xml_type << std::endl;
//						std::cout << "label: " << label << std::endl;
//						std::cout << "id: " << id << std::endl;
//						std::cout << "activation: " << activation_function << std::endl;



						if(xml_type == XML_TYPE_HIDDEN){
							type = TYPE_HIDDEN;
						} else {
							if (xml_type == XML_TYPE_INPUT) {
								type = TYPE_INPUT;
							} else if (xml_type == XML_TYPE_OUTPUT){
								type = TYPE_OUTPUT;
							} else {
								throw JGTL::LocatedException("File contains unknown node type: '" + xml_type +"'");
							}
							io_map[label] = id;
						}

						if(activation_function == XML_GAUSSIAN){
							activation_function_enum = ACTIVATION_FUNCTION_GAUSSIAN;
						} else if(activation_function == XML_LINEAR){
							activation_function_enum = ACTIVATION_FUNCTION_LINEAR;
						} else if(activation_function == XML_SIN){
							activation_function_enum = ACTIVATION_FUNCTION_SIN;
						} else if(activation_function == XML_SIGMOID){
							activation_function_enum = ACTIVATION_FUNCTION_SIGMOID;
						} else if(activation_function == XML_COS){
							activation_function_enum = ACTIVATION_FUNCTION_COS;
						} else {
							throw JGTL::LocatedException("File contains unknown activation function: '" + activation_function + "'");
//							ParseException e;
//							e << err_msg("Unknown activation function " + activation_function + " in: " + fileName);
//							throw e;
//							throw ParseException();
						}


						nodeVector.push_back(NEAT::GeneticNodeGene(
								id,
								type,
								drawing_posistion,
								RANDOM_ACTIVATION_FALSE,
								activation_function_enum)
						);





					} else if(parsing_link){
						if(first_link){
							cppn = new NEAT::GeneticIndividual(
									nodeVector,
									CREATE_TOPOLOGY_FALSE,
									EDGE_DENSITY_NOT_CONNECTED
							);
							first_link = false;
						}


//						std::cout << "link" << std::endl;
//						std::cout << "id: " << id << std::endl;
//						std::cout << "source: " << source_id <<std::endl;
//						std::cout << "target: " << target_id <<std::endl;
//						std::cout << "weight: " << std::fixed << weight_f << std::endl;

						int fromNodeID = cppn->getNode(source_id)->getID();
						int toNodeID = cppn->getNode(target_id)->getID();

						cppn->addLink(
								NEAT::GeneticLinkGene(
										fromNodeID,
										toNodeID,
										weight_f
								)
						);
					}

					parsing_node=false;
					parsing_link=false;
					done_parsing=false;
					label = "";
					activation_function = "";
					id = "";
					source_id = "";
					target_id = "";
					weight_f = 0.0;
				}


			}
			myfile.close();
		} else{
			throw JGTL::LocatedException("File not readable");
		}

		if(!cppn){
			cppn = new NEAT::GeneticIndividual(
					nodeVector,
					CREATE_TOPOLOGY_FALSE,
					EDGE_DENSITY_NOT_CONNECTED
			);
		}


		return shared_ptr<NEAT::GeneticIndividual>(cppn);
	}

};

#endif /* CPPNPARSER_H_ */
