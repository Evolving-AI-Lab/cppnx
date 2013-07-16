/*
 * CE_LabelWidget.h
 *
 *  Created on: Jul 2, 2013
 *      Author: joost
 */

#ifndef CE_LABELWIDGET_H_
#define CE_LABELWIDGET_H_

#include <QWidget>
#include "CE_Label.h"
#include "CE_CommandLabelObject.h"
#include "CE_CommandAddLabel.h"
#include "CX_DragAndDropGraphicsView.h"

//class VerticalScrollArea;
class QLineEdit;
class QSignalMapper;
class QPushButton;
class QVBoxLayout;
class QAction;
class QMenu;

class LabelWidget: public  QWidget {
	Q_OBJECT

public slots:
	void addLabel();
	void requestRemoveLabel(Label* label);
	void requestDelete();
	void unlabel();
	void labelableObjectSelected(bool selected);
//	void indexChanged(uint index);
//	void objectsMoved();
	void rebuildMenu();
	void labelSelected(bool labelSelected);
	void updateSelection();

public:
	LabelWidget();
	virtual ~LabelWidget();

	void addLabel(Label* label);
	void setLabels(QList<Label*> labels);
	void removeLabel(Label* labelWidget);
	void clear();

	int getNrOfLabels();
	Label* getLabel(size_t i);
	QList<Label*> getLabels();


	QAction* getAddLabelAction(){
		return addLabelAction;
	}

	QAction* getUnlabelAction(){
		return unlabelAction;
	}

	QMenu* getLabelMenu(){
		return labelMenu;
	}

signals:
	void requestCommandExecution(QUndoCommand*);
	void applyLabel(Label*);
	void labelsChanged();
	void sceneModified();


private:
//	void setIds();

    QPushButton* addLabelButton;
    QPushButton* unlabelButton;
    QLineEdit *labelName;

    QVBoxLayout* colorMainLayout;
//    QVBoxLayout* colorLabelLayout;
//    VerticalScrollArea* labelBar;
    DragAndDropGraphicsView* dragAndDropLabelBar;

//    QSignalMapper* deleteSignalMapper;
//    QSignalMapper* colorSignalMapper;

    QAction *addLabelAction;
    QAction *unlabelAction;
    QAction* deleteAction;

    QMenu* labelMenu;

    bool labelIsSelected;
};

#endif /* CE_LABELWIDGET_H_ */
