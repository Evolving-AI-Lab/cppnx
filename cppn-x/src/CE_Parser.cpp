/*
 * CE_Parser.cpp
 *
 *  Created on: Nov 11, 2014
 *      Author: Joost Huizinga
 */

#include "CE_Parser.hpp"

/**
 * If the template string is of the form: "key arg1 arg2 arg3"
 * generates a string in the form: <key arg1="*" arg2="*" arg3="*">*</key>.
 *
 * @param template_str The template string of the form: "key arg1 arg2 arg3".
 * @return             A string of the form <key arg1="*" arg2="*" arg3="*">*</key>.
 */
std::string openCloseXml(std::string template_str){
	return std::string(ce_xml::getOpenXmlString(template_str) + "*" + ce_xml::getCloseXmlString(template_str));
}

/**
 * If the template string is of the form: "key arg1 arg2 arg3"
 * generates a string in the form: <key arg1="*" arg2="*" arg3="*">.
 *
 * @param template_str The template string of the form: "key arg1 arg2 arg3".
 * @return             A string of the form <key arg1="*" arg2="*" arg3="*">.
 */
std::string openXml(std::string template_str){
	return std::string(ce_xml::getOpenXmlString(template_str, "*"));
}

/**
 * If the template string is of the form: "key arg1 arg2 arg3"
 * generates a vector containing strings of the form:
 * - <key arg1="*" arg2="*" arg3="*">
 * - <key arg1="*" arg2="*" arg3="*"/>
 *
 * @param template_str The template string of the form: "key arg1 arg2 arg3".
 * @return             A vector of strings of the form <key arg1="*" arg2="*" arg3="*">.
 */
std::vector<std::string> openReadXml(std::string template_str){
    std::vector<std::string> matches;
    matches.push_back(ce_xml::getOpenXmlString(template_str, "*"));
    matches.push_back(ce_xml::getOneLineXmlString(template_str, "*"));
    return matches;
}

/**
 * If the template string is of the form: "key arg1 arg2 arg3"
 * generates a string in the form: <key arg1="*" arg2="*" arg3="*"/>.
 *
 * @param template_str The template string of the form: "key arg1 arg2 arg3".
 * @return             A string of the form <key arg1="*" arg2="*" arg3="*"/>.
 */
std::string readXml(std::string template_str){
	return std::string(ce_xml::getOneLineXmlString(template_str, "*"));
}

/**
 * If the template string is of the form: "key arg1 arg2 arg3"
 * generates a string in the form: </key>.
 *
 * @param template_str The template string of the form: "key arg1 arg2 arg3".
 * @return             A string of the form </key>.
 */
std::string closeXml(std::string template_str){
	return std::string(ce_xml::getCloseXmlString(template_str));
}


