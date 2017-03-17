/*
 * CX_GlobalSettings.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: Joost Huizinga
 */

#include "CX_GlobalSettings.h"

namespace glb_settings{

GlobalSettings* globalSettings = 0;

void initGlobalSettings(){
    if(globalSettings == 0){
        globalSettings = new GlobalSettings();
    }
}

void uninitGlobalSettings(){
    if(globalSettings != 0){
        delete globalSettings;
        globalSettings = 0;
    }
}
}
