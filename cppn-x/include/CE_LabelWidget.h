/*
 * CE_LabelWidget.h
 *
 *  Created on: Jun 3, 2013
 *      Author: joost
 */

#ifndef CE_COLORBUTTON_H_
#define CE_COLORBUTTON_H_

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QAction>
#include <QHBoxLayout>
#include <QFrame>


#include "CE_Defines.h"
#include "CE_Util.h"
#include "CE_Window.h"


class QLabel;
class QPushButton;
class QObject;
class QHBoxLayout;
class Window;


class LabelWidget : public QFrame{
	Q_OBJECT

public:

	LabelWidget(Window* window, QString text = "", QColor color = Qt::white, bool isDeleted = true, QWidget *parent = 0);
	virtual ~LabelWidget();

	QLabel* getLabel(){
		return label;
	}
	QPushButton* getColorButton(){
		return colorButton;
	}
	QPushButton* getDeleteButton(){
		return deleteButton;
	}

	QColor getColor(){
		return color;
	}

	void setColor(QColor _color);

	QString getText() const;
//	std::string getText() const;

	void setText(const QString& name);

	QAction* getColorAction(){
		return colorAction;
	}

	QAction* getDeleteAction(){
		return deleteAction;
	}

	QAction* getRenameAction(){
		return deleteAction;
	}

	QAction* getChangeColorAction(){
		return deleteAction;
	}

	void registerObject(){
		registerdObjects++;
	}

	void unregisterObject(){
		registerdObjects--;

		if(registerdObjects==0){
			delete this;
		}
	}

	bool isDeleted(){
		return deleted;
	}

	void setDeleted(bool _deleted = true){
		deleted = _deleted;
		if(deleted){
			hide();
			colorAction->setShortcut(tr(""));
		} else {
			show();
		}
	}

	void setId(id_t _id){
		id = _id;
		if(id>0){
			QString shortcut = "Alt+" + util::toQString(id);
			colorAction->setShortcut(shortcut);
		}
	}

	id_t getId(){
		return id;
	}

	unsigned int registerdObjects;

	void contextMenuEvent(QContextMenuEvent *event);

	void setHighlightOn();
	void setHighlightOff();

	QString baseSs;
	QString selectedSs;
	QString highlightSs;
	QString selectedHighlightSs;

public slots:
void changeLabelColor();
void changeLabelName();

protected:
void focusInEvent(QFocusEvent* event );
void focusOutEvent(QFocusEvent* event );

private:
	Window* window;
	QLabel* label;
	QPushButton* colorButton;
	QPushButton* deleteButton;
	QHBoxLayout* colorButtonLayout;
	QColor color;
	QAction* colorAction;
	QAction* deleteAction;
	QAction* changeColorAction;
	QAction* renameAction;



	id_t id;
	bool deleted;
	bool highlight;
};

#endif /* CE_COLORBUTTON_H_ */
