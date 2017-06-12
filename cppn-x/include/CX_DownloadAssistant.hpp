/*
 * CX_DownloadAssistant.hpp
 *
 *  Created on: May 4, 2017
 *      Author: joost
 */

#ifndef CPPN_X_INCLUDE_CX_DOWNLOADASSISTANT_HPP_
#define CPPN_X_INCLUDE_CX_DOWNLOADASSISTANT_HPP_

#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


class DownloadAssistant : public QObject {
    Q_OBJECT
public:
    explicit DownloadAssistant(const QString& url="", const QString& dest="");
    ~DownloadAssistant();

    void setTarget(const QString& t);
    void setDestination(const QString& dest);

private:
    QNetworkAccessManager _manager;
    QString _target;
    QString _dest;
    bool _success;

signals:
    void done();

public slots:
    bool download();
    void downloadFinished(QNetworkReply* data);
    void downloadProgress(qint64 recieved, qint64 total);
};



#endif /* CPPN_X_INCLUDE_CX_DOWNLOADASSISTANT_HPP_ */
