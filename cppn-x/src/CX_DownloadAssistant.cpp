/*
 * CX_DownloadAssistant.cpp
 *
 *  Created on: May 4, 2017
 *      Author: joost
 */

#include "CX_DownloadAssistant.hpp"

#include <QUrl>
#include <QNetworkRequest>
#include <QFile>
#include <QEventLoop>

#include <iostream>

DownloadAssistant::DownloadAssistant(const QString& url, const QString& dest):
QObject(0)
{
    QObject::connect(&_manager, SIGNAL(finished(QNetworkReply*)),
    		this, SLOT(downloadFinished(QNetworkReply*)));
    _success = false;
    _target = url;
    _dest = dest;
}

DownloadAssistant::~DownloadAssistant() {

}


void DownloadAssistant::setTarget(const QString &t) {
	_target = t;
}

void DownloadAssistant::setDestination(const QString &dest) {
	_dest = dest;
}


void DownloadAssistant::downloadFinished(QNetworkReply *data) {
    QFile localFile(_dest);
    if (!localFile.open(QIODevice::WriteOnly)){
    	std::cout << "ERROR: Could not open file for writing." << std::endl;
    	emit done();
        return;
    }
    if(data->error() != QNetworkReply::NoError){
    	std::cout << "ERROR: " << data->error() << std::endl;
    	emit done();
    	return;
    }
    const QByteArray sdata = data->readAll();
    localFile.write(sdata);
    localFile.close();
    _success = true;
    emit done();
}

bool DownloadAssistant::download() {
	_success = false;
    QUrl url(_target);
    QNetworkRequest request(url);
    QNetworkReply* reply = _manager.get(request);
    QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
    		this, SLOT(downloadProgress(qint64,qint64)));
    QEventLoop loop;
    connect(this, SIGNAL(done()), &loop, SLOT(quit()));
    loop.exec();
    return _success;
}

void DownloadAssistant::downloadProgress(qint64 recieved, qint64 total) {

}

