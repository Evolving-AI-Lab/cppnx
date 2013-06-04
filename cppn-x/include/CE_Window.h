/*
 * window.h
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <QDialog>
#include "CE_CppnWidget.h"
#include "CE_ColorButton.h"
#include <QtGui>
#include <iostream>


QT_BEGIN_NAMESPACE
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE

class CE_ColorButton;
class GraphWidget;

//! [0]
class Window : public QDialog
{
    Q_OBJECT

public slots:
    void setValue(int _value);
    void load();
    void save();
    void addColorButton();
    void deleteColorButton(QObject* object);

public:
    Window();
//    ~Window();
    void addColorButton(QString text, QColor color);
    size_t getNrOfColorButtons();
    CE_ColorButton* getColorButton(size_t i);
    void clearColorButtons();

    void showScrubberBar(bool visible);
    void showColorBar(bool visible);

private:
    void createMenu();
    void createHorizontalGroupBox();
    void createGridGroupBox();

    QMenuBar *menuBar;
    QGroupBox *horizontalGroupBox;
    QGroupBox *gridGroupBox;
    QMenu *fileMenu;
    QAction *exitAction;
    QAction *loadAction;
    QAction *saveAction;

    enum { NumGridRows = 2, NumButtons = 4 };
//    std::vector<QLabel*> labels;
//    std::vector<QPushButton*> colorButtons;
//    std::vector<QPushButton*> deleteButtons;
    QGridLayout *colorLayout;
    QLineEdit *labelName;
    QSignalMapper* deleteSignalMapper;
    QSignalMapper* colorSignalMapper;
    QList<CE_ColorButton*> buttons;

    GraphWidget *graphWidget;
    QSlider* slider;
    QDoubleSpinBox *spinBox;

    int my_value;
};
//! [0]


#endif /* WINDOW_H_ */
