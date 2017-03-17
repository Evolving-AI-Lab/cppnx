/*
 * CX_GlobalSettings.h
 *
 *  Created on: Dec 18, 2016
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_GLOBALSETTINGS_H_
#define CPPN_X_INCLUDE_CX_GLOBALSETTINGS_H_

// QT includes
#include <QSettings>
#include <QApplication>

// Local includes
#include "CX_Debug.hpp"
namespace glb_settings{

class GlobalSettings{
public:
    GlobalSettings(){
        //nix
    }

    void load(){
        dbg::trace trace("settings", DBG_HERE);
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "EvolvingAILab", "CppnExplorer");
        _genomeDir = settings.value("directories/genome", "").toString();
        dbg::out(dbg::info, "settings") << "Genome dir: "<< _genomeDir.toStdString() << std::endl;
    }

    void save(){
        dbg::trace trace("settings", DBG_HERE);
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "EvolvingAILab", "CppnExplorer");
        dbg::out(dbg::info, "settings") << "Genome dir: "<< _genomeDir.toStdString() << std::endl;
        settings.setValue("directories/genome", _genomeDir);
    }

    const QString& getGenomeDir() const{ return _genomeDir;}
    void setGenomeDir(const QString& genomeDir) { _genomeDir = genomeDir;}

private:
    QString _genomeDir;
};

extern GlobalSettings* globalSettings;
void initGlobalSettings();
void uninitGlobalSettings();


}


#endif /* CPPN_X_INCLUDE_CX_GLOBALSETTINGS_H_ */
