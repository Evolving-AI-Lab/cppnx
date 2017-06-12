/*
 * CX_WeightWidget.h
 *
 *  Created on: Jul 5, 2013
 *      Author: joost
 */

#ifndef CX_WEIGHTWIDGET_H_
#define CX_WEIGHTWIDGET_H_


#include <QGroupBox>
#include <QAction>
#include <QDoubleSpinBox>

#include "CX_WeightSliderWidget.h"
#include "CE_Node.h"
#include "CE_Edge.h"

class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
//class QDoubleSpinBox;
class QCheckBox;

class WeightWidget: public QGroupBox {
	 Q_OBJECT
public:
	WeightWidget(QWidget* parent = 0);
	virtual ~WeightWidget();

	//Getters and setters
	double getValue(){return weightSliderWidget->getValue();}
	double getScanStartValue(){return scanStartSpinBox->value();}
	double getScanStopValue(){return scanStopSpinBox->value();}
	double getStepSizeValue(){return stepSizeSpinBox->value();}

	void setValue(double value);
	void setEdge(double weight, double originalWeight, QString id, double bookendStart, double bookendEnd, double bookendStep);
	void setNode(QString id);

	WeightSliderWidget* getFirstWeightSliderWidget(){return weightSliderWidget;}

    QAction* getResetAllAction(){return resetAllAction;}
    QAction* getScanAction(){return scanAction;}
    QAction *getScreenCaptureAction(){return screenCaptureAction;}

    bool isCapturing(){return capturing;}
    bool isScanning(){return scanning;}

    void edgeSelected(bool selected);

public slots:
	void startStopFilm();
	void startFilm();
    void startScan();
    void stopScan();
    void updateEdge(Edge* edge);
    void updateNode(Node* node);

signals:
	void scanStarted();
	void scanStopped();
	void filmStarted();
	void filmStopped();
	void frameReady();
	void requestFilmStart();

	void bookendStartChanged(double);
	void bookendEndChanged(double);
	void bookendStepChanged(double);

	void flash(bool);

protected:
	void timerEvent(QTimerEvent *event);
    void changeEvent(QEvent* event);

private:
	void scanStarted(bool selected);
	void filmStarted(bool selected);

	bool scanning;
	bool capturing;
	bool edgeIsSelected;

	int scanState;
	double start;

	WeightSliderWidget* weightSliderWidget;

    QAction* resetAllAction;
    QAction* scanAction;
    QAction *screenCaptureAction;

    QPushButton* resetAll;
    QPushButton* scan;
    QPushButton* scanAndCapture;
    QPushButton* startHereButton;

    QHBoxLayout* mainLayout;
    QVBoxLayout* weightSlidersLayout;
    QHBoxLayout* buttonLayout;

    QVBoxLayout* resetStartHereLayout;
    QVBoxLayout* scanFilmLayout;
    QVBoxLayout* stepLayout;
    QVBoxLayout* startLayout;
    QVBoxLayout* endLayout;

    QLabel* stepSizeLabel;
    QLabel* scanStartLabel;
    QLabel* scanStopLabel;

//    QCheckBox* startHereCheckBox;

    QDoubleSpinBox *stepSizeSpinBox;
    QDoubleSpinBox *scanStartSpinBox;
    QDoubleSpinBox *scanStopSpinBox;
    int timerId;

};

#endif /* CX_WEIGHTWIDGET_H_ */
