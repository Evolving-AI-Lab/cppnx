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
#include <QGraphicsLinearLayout>


#include "CE_Defines.h"
#include "CE_Util.h"
#include "CE_CommandChangeLabelColor.h"
#include "CE_CommandChangeLabelName.h"
#include "CX_DragAndDropObject.h"


//#include "CE_Window.h"


class QLabel;
class QPushButton;
class QObject;
class QHBoxLayout;
//class Window;


class Label : public DragAndDropObject{
	Q_OBJECT

public:

	Label(QString text = "", QColor color = Qt::white, bool isDeleted = true);
	Label(std::iostream &stream);

	virtual ~Label();

	void init();

    enum { Type = UserType + LABEL_TYPE };
    virtual int type() const { return Type; }

//	QLabel* getLabel(){
//		return label;
//	}
	QPushButton* getColorButton(){
		return colorButton;
	}
//	QPushButton* getDeleteButton(){
//		return deleteButton;
//	}

	QColor getColor(){
		return color;
	}

	void setColor(QColor _color);

	QString getText() const{
		return labelName;
	}
//	std::string getText() const;

	void setText(const QString& name){
		labelName = name;
		shortText.clear();
	}

	QAction* getColorAction(){
		return colorAction;
	}

//	QAction* getDeleteAction(){
//		return deleteAction;
//	}
//
//	QAction* getRenameAction(){
//		return deleteAction;
//	}
//
//	QAction* getChangeColorAction(){
//		return deleteAction;
//	}

	void registerObject(){
//		std::cout << "Label: " << this << " Registerd: " << registerdObjects << std::endl;
		registerdObjects++;
//		std::cout << "Label: " << this << " Registerd: " << registerdObjects << std::endl;
	}

	void unregisterObject(){
//		std::cout << "Label: " << this << " Registerd: " << registerdObjects << std::endl;
		registerdObjects--;
//		std::cout << "Label: " << this << " Unregisterd: " << registerdObjects << std::endl;

		if(registerdObjects==0){
//			std::cout << "Destroyed" <<std::endl;
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
			QString shortcut = "Alt+" + util::toQString(index+1);
			colorAction->setShortcut(shortcut);
		}
	}

//	void setId(id_t _id){
//////		id = _id;
////		if(index+1>0){
//		QString shortcut = "Alt+" + util::toQString(index+1);
//		colorAction->setShortcut(shortcut);
////		}
//	}

	id_t getId(){
		if(deleted){
			return 0;
		} else {
			return getIndex() + 1;
		}
	}

	unsigned int registerdObjects;

//	void contextMenuEvent(QContextMenuEvent *event);

	void setHighlightOn();
	void setHighlightOff();

//	QString baseSs;
//	QString selectedSs;
//	QString highlightSs;
//	QString selectedHighlightSs;

public slots:
	void changeLabelColor();
	void changeLabelName();
	void applyLabel();
	void deleteLabel();

signals:
	void requestCommandExecution(QUndoCommand*);
	void labelChanged();
	void applyLabel(Label*);
	void deleteLabel(Label*);

protected:
//	void focusInEvent(QFocusEvent* event );
//	void focusOutEvent(QFocusEvent* event );
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

private:
	QPushButton* colorButton;
	QGraphicsLinearLayout* colorButtonLayout;
	QColor color;
	QAction* colorAction;
	QAction* changeColorAction;
	QAction* renameAction;
	QString labelName;
	QString shortText;

	QRectF boundingBox;

	id_t id;
	bool deleted;
	bool highlight;
};

#endif /* CE_COLORBUTTON_H_ */
