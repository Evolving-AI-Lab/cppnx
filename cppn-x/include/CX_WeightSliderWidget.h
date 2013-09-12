/*
 * CX_WeightSliderWidget.h
 *
 *  Created on: Jul 5, 2013
 *      Author: joost
 */

#ifndef CX_WEIGHTSLIDERWIDGET_H_
#define CX_WEIGHTSLIDERWIDGET_H_

#include <QWidget>
#include <QAction>

class QSlider;
class QDoubleSpinBox;
class QHBoxLayout;
class QPushButton;
class QLabel;

class WeightSliderWidget: public QWidget {
	 Q_OBJECT
public:
	WeightSliderWidget(QWidget* object = 0);
	virtual ~WeightSliderWidget();

	QAction* getResetAction(){
		return resetAction;
	}

	double getValue();

	void resetFlash(){
		flashCounter = 0;
	}
	bool flash();

public slots:
	void setEdge(double weight, double originalWeight, QString id);
	void setNode(QString id);
	void reset();
	void setSlider(double weight);
	void setSlider(int value);
	void setStepSize(double stepSize);

signals:
	void weightChanged(double);

private:
	void setSlider(int value, double weight);
	int sliderBase;
	int sliderRange;
	int flashCounter;

	QLabel* idLabel;
    QSlider* slider;
    QDoubleSpinBox *spinBox;
    QHBoxLayout* layout;

    QPushButton* resetButton;

    QAction* resetAction;

    double resetWeight;

};

#endif /* CX_WEIGHTSLIDERWIDGET_H_ */
