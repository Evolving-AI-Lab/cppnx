/*
 * CE_LabelWidget.h
 *
 *  Created on: Jul 2, 2013
 *      Author: joost
 */

#ifndef CE_LABELWIDGET_H_
#define CE_LABELWIDGET_H_

#include <CX_ComAddLabel.h>
#include <CX_ComLabelObject.h>
#include <QWidget>
#include "CE_Label.h"
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
	void rebuildMenu();
	void labelSelected(bool labelSelected);
	void singleLabelSelected(bool singleLabelSelected);
	void updateSelection();
	void rename();
	void recolor();
	void applyLabel();

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

	QMenu* getLabelContextMenu(){
		return labelContextMenu;
	}

	QMenu* getNodeContextMenu(){
		return nodeContextMenu;
	}

	bool getLabelSelected(){
		return labelIsSelected;
	}

	bool getGraphicsViewHasFocus(){
		return dragAndDropLabelBar->getIsActive();
	}

	void setDeleteAction(QAction* _deleteAction){
		deleteAction = _deleteAction;
	}

signals:

    /**
     * The LabelWidget will emit this signal when a command is executed
     */
	void requestCommandExecution(ComBase*);

	/**
	 * The LabelWidget will emit this signal when a label is applied
	 */
	void applyLabel(Label*);

	/**
	 * The LabelWidget will emit this signal when:
	 * - a label is added
	 * - a label is removed
	 * - the color of a label is changed
	 */
	void labelsChanged();
//	void sceneModified();

	/**
	 * The LabelWidget will emit this signal when:
	 * - any of the labels is selected or deselected
	 */
	void selectionChanged();


private:
//	void setIds();

    QPushButton* addLabelButton;
    QPushButton* unlabelButton;
    QLineEdit *labelName;

    QVBoxLayout* colorMainLayout;
    DragAndDropGraphicsView* dragAndDropLabelBar;

    QAction *addLabelAction;
    QAction *unlabelAction;
    QAction* deleteAction;
	QAction* changeColorAction;
	QAction* renameAction;
	QAction* applyLabelAction;

    QMenu* labelMenu;
    QMenu* labelContextMenu;
    QMenu* nodeContextMenu;

    bool labelIsSelected;
    bool singleLabelIsSelected;
    bool labelableObjectIsSelected;
};

#endif /* CE_LABELWIDGET_H_ */
