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
    GlobalSettings():
    	_downloadGenomes(false),
		_downloadPrefSet(false)
	{
        //nix
    }

    void load(){
        dbg::trace trace("settings", DBG_HERE);
        QSettings settings(QSettings::IniFormat, QSettings::UserScope,
        		"EvolvingAILab", "CppnExplorer");
        _genomeDir = settings.value("directories/genome", "").toString();
        _downloadGenomes = settings.value("archive/download", "").toBool();
        _downloadPrefSet = settings.value("archive/downloadSet", "").toBool();
        dbg::out(dbg::info, "settings") << "Genome dir: "<<
        		_genomeDir.toStdString() << std::endl;
    }

    void save(){
        dbg::trace trace("settings", DBG_HERE);
        QSettings settings(QSettings::IniFormat, QSettings::UserScope,
        		"EvolvingAILab", "CppnExplorer");
        dbg::out(dbg::info, "settings") << "Genome dir: "<<
        		_genomeDir.toStdString() << std::endl;
        settings.setValue("directories/genome", _genomeDir);
        settings.setValue("archive/download", _downloadGenomes);
        settings.setValue("archive/downloadSet", _downloadPrefSet);
    }

    const QString& getGenomeDir() const{ return _genomeDir;}
    void setGenomeDir(const QString& genomeDir) { _genomeDir = genomeDir;}
    bool getDownloadPref() const { return _downloadGenomes; }
    void setDownloadPref(bool pref){ _downloadGenomes = pref; }
    bool getDownloadPrefSet() const { return _downloadPrefSet; }
    void setDownloadPrefSet(bool pref){ _downloadPrefSet = pref; }
private:
    QString _genomeDir;
    bool _downloadGenomes;
    bool _downloadPrefSet;
};

extern GlobalSettings* globalSettings;
void initGlobalSettings();
void uninitGlobalSettings();


}


#endif /* CPPN_X_INCLUDE_CX_GLOBALSETTINGS_H_ */
