/*
 * CX_ModuleColor.h
 *
 *  Created on: Jan 17, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_MODULECOLOR_H_
#define CPPN_X_INCLUDE_CX_MODULECOLOR_H_

#include <vector>
#include <QColor>

namespace cx_colors{

void initModuleColors();

QColor getModuleColor(size_t module);

}
#endif /* CPPN_X_INCLUDE_CX_MODULECOLOR_H_ */
