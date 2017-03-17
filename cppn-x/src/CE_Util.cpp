/*
 * CE_Util.cpp
 *
 *  Created on: Sep 6, 2013
 *      Author: joost
 */

#include <QGraphicsItem>
#include <QImageWriter>
#include <QList>
#include <QByteArray>
#include <QFileDialog>
#include <QMenu>
#include <QAction>

#include "CE_Util.h"

QString util::getSupportedExtentions(){
	QList<QByteArray> formats  = QImageWriter::supportedImageFormats();
	QList<QString> orderdFormats;
	QString fileNames("");

	foreach(QByteArray format, formats){
		QString temp = QString(format);
		if(temp == "jpg" || temp == "jpeg" || temp == "png"){
			orderdFormats.push_front(temp);
		} else {
			orderdFormats.push_back(temp);
		}
	}

	foreach(QString temp, orderdFormats){
		fileNames.append(temp);
		fileNames.append(" file (*.");
		fileNames.append(temp);
		fileNames.append(");;");
	}

	fileNames.append("All Files (*)");
	return fileNames;
}


QString util::getSupportedFilename(QWidget* parent, const QString& text){
	QString fileNames = util::getSupportedExtentions();
	QString newFileName = QFileDialog::getSaveFileName(parent, text, "",fileNames);
	return newFileName;
}

QString util::getBase(const QString& fileName){
	return fileName.left(fileName.lastIndexOf('.'));
}

QString util::getNameFromBase(const QString& fileName){
    return fileName.mid(fileName.lastIndexOf('/'));
}

QString util::getExtention(const QString& fileName){
	return fileName.mid(fileName.lastIndexOf('.'));
}

void util::insertFront(QMenu* menu, QAction* action){
	if(menu->actions().size() > 0){
		menu->insertAction(menu->actions().front(), action);
	} else {
		menu->addAction(action);
	}
}

size_t util::factorial(size_t n){
    size_t result = n;
    while(n > 1){
        --n;
        result*=n;
    }
    return result;
}

size_t util::combinations(size_t n, size_t k){
    return factorial(n)/(factorial(k)*factorial(n-k));
}
