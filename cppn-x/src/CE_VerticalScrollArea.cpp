/*
 * CE_VerticalScrollArea.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: joost
 */

#include "CE_VerticalScrollArea.h"
#include <QEvent>
#include <QVBoxLayout>
#include <QScrollBar>

VerticalScrollArea::VerticalScrollArea(): QScrollArea() {
	 setWidgetResizable(true);
//	 setFrameStyle(QFrame::NoFrame);
	 setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	 setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	 m_scrollAreaWidgetContents = new QWidget(this);
	 m_scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//	 QVBoxLayout *baseLayout = new QVBoxLayout(m_scrollAreaWidgetContents);
	 setWidget(m_scrollAreaWidgetContents);
	 m_scrollAreaWidgetContents->installEventFilter(this);
}


VerticalScrollArea::VerticalScrollArea(QWidget *parent): QScrollArea(parent) {
	 setWidgetResizable(true);
//	 setFrameStyle(QFrame::NoFrame);
	 setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	 setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	 m_scrollAreaWidgetContents = new QWidget(this);
	 m_scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//	 QVBoxLayout *baseLayout = new QVBoxLayout(m_scrollAreaWidgetContents);
	 setWidget(m_scrollAreaWidgetContents);
	 m_scrollAreaWidgetContents->installEventFilter(this);
}

bool VerticalScrollArea::eventFilter(QObject *o, QEvent *e)
{
 if(o == m_scrollAreaWidgetContents && e->type() == QEvent::Resize)
  setMinimumWidth(m_scrollAreaWidgetContents->minimumSizeHint().width() + verticalScrollBar()->width());

 return false;
}


//VerticalScrollArea::~VerticalScrollArea() {
//	// TODO Auto-generated destructor stub
//}

