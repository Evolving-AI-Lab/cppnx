/*
 * CX_ModuleColor.cpp
 *
 *  Created on: Jan 17, 2015
 *      Author: Joost Huizinga
 */

#include "CX_ModuleColor.h"

std::vector<QColor> moduleColors;

void cx_colors::initModuleColors(){
    moduleColors.push_back(QColor(0, 0, 133));
    moduleColors.push_back(QColor(133, 0, 0));
    moduleColors.push_back(QColor(0, 133, 133));
    moduleColors.push_back(QColor(255, 200, 0));
    moduleColors.push_back(QColor(0, 133, 255));
    moduleColors.push_back(QColor(255, 255, 0));
    moduleColors.push_back(QColor(0, 133, 0));
    moduleColors.push_back(QColor(255, 200, 160));
}

QColor cx_colors::getModuleColor(size_t module){
    if(module < moduleColors.size()){
        return moduleColors[module];
    }
    QColor moduleColor;
    moduleColor.setHsv(module*100+100, 255, 255);
    return moduleColor;
}



