/*
 * CE_VerticalScrollArea.h
 *
 *  Created on: Jun 26, 2013
 *      Author: joost
 */

#ifndef CE_VERTICALSCROLLAREA_H_
#define CE_VERTICALSCROLLAREA_H_

#include <QScrollArea>

class VerticalScrollArea: public QScrollArea {
public:
	VerticalScrollArea();
	VerticalScrollArea(QWidget *parent);
	QWidget* m_scrollAreaWidgetContents;

protected:
	bool eventFilter(QObject *o, QEvent *e);
//	virtual ~VerticalScrollArea();


};

#endif /* CE_VERTICALSCROLLAREA_H_ */
