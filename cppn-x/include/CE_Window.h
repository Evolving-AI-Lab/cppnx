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
#include "CE_LabelWidget.h"
#include "CE_NodeView.h"
#include "CE_Node.h"
#include "CE_VerticalScrollArea.h"

#include <QtGui>
#include <iostream>

#ifdef USE_FFMPEG
#include "QVideoEncoder.h"
#endif //USE_FFMPEG


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

class LabelWidget;
class GraphWidget;
class Node;
class Edge;
class NodeView;

//! [0]
class Window : public QWidget
{
    Q_OBJECT

public slots:
    void load();
    void save();
    void saveAs();
    void addColorButton();
    void deleteColorButton(QWidget* object);
    void addNodeView();
    void deleteNodeView();
    void unlabel();
    void captureScreen();
    void startScan();
    void stopScan();
    void colorNode(QWidget* object);

    void resetWeight();
    void resetAllWeights();
    void setValue(int value);
    void setValueF(double value);

    void updateSidebarSelection();
    void updateMainSelection();

    void setLabelOny();
    void setSignOny();
    void setBoth();


public:
    Window();
    virtual ~Window();
//    void addColorButton(QString text, QColor color);
    void addLabelWidget(LabelWidget* labelWidget);
    void removeLabelWidget(LabelWidget* labelWidget);

    size_t getNrOfColorButtons();
    LabelWidget* getColorButton(size_t i);
    void clearColorButtons();

    void closeEvent(QCloseEvent * event);

    void setCppn(Cppn* _cppn){
    	cppn = _cppn;
    }

    void clearNodeViews();
    void actualSave(const QString& fileName);

    enum labelModes {onlyLabels, onlyConnectionSign, both};
    int labelMode;

signals:
    void sliderValueChanged(int newValue);
    void sliderValueChangedF(double newValue);



protected:
    void timerEvent(QTimerEvent *event);

private:
    //Constructor functions
    void createMenu();
    void createWeightBar();
    void createLabelBar();
    void createNodeViewBar();

    void resetWeights(QList<QGraphicsItem*> items, bool batch);
    void setNodeviewPosition(NodeView* node, size_t index);
    void setNodeviewPositions();
    void setSidebarSceneRect();
    void addNodeView(Node* node);
    void deleteNodeView(NodeView* nodeToDelete);

    void captureFrame();
    void stopCapture();
    void nodeViewSelected(bool selected);
    void nodeSelected(bool selected);
    void edgeSelected(bool selected, Edge* edge);
    void colorNode(LabelWidget* color);
    void setSlider();

    Cppn* cppn;
    QMenuBar *menuBar;
    QGroupBox *horizontalGroupBox;
    QGroupBox *gridGroupBox;
    QGraphicsView* sidebar;
    VerticalScrollArea* labelBar;


    QMenu *fileMenu;
    QMenu* editMenu;
    QMenu* posMenu;
    QMenu* labelMenu;
    QMenu* viewMenu;
    QAction *exitAction;
    QAction *loadAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction* resetAction;
    QAction* resetAllAction;
    QAction* circleAction;
    QAction* layerAction;
    QAction* scanAction;
    QAction* addViewNodeAction;
    QAction* deleteViewNodeAction;
    QAction *addLabelAction;
    QAction *unlabelAction;
    QAction *screenCaptureAction;

    QAction *undoAction;
    QAction *redoAction;

    QAction *labelOnlyAction;
    QAction *signOnlyAction;
    QAction *labelAndSignAction;

    QString currentFileName;

//    std::vector<QLabel*> labels;
//    std::vector<QPushButton*> colorButtons;
//    std::vector<QPushButton*> deleteButtons;
//    QGridLayout *colorLayout;
//    QGridLayout *colorLayout;
    QHBoxLayout *mainLayout;
    QVBoxLayout* colorMainLayout;
    QVBoxLayout* colorLabelLayout;
    QWidget* colorLabelWidget;
//    QVBoxLayout* colorRightLayout;
//    FlowLayout* colorLeftLayout;

    QPushButton* addLabel;
    QPushButton* unlabelButton;
    QLineEdit *labelName;

    QSignalMapper* deleteSignalMapper;
    QSignalMapper* colorSignalMapper;
    QList<LabelWidget*> buttons;
    QList<NodeView*> nodeViews;

    GraphWidget *graphWidget;
    QSlider* slider;
    QDoubleSpinBox *spinBox;

    bool updateEdges;

    static const int sidebarMargin = 20;
    static const int betweenNodeMargin = 20;

    bool edgeIsSelected;
    Edge* selectedEdge;
    bool nodeIsSelected;

    int timerId;
    bool capture;
    Edge* scannedEdge;
    QString captureDirectory;
    QList<NodeView*> nodeViewsToBeCaptured;

    QUndoStack undoStack;

#ifdef USE_FFMPEG
    QList<QVideoEncoder*> nodeViewEncoders;
    QVideoEncoder encoder;
#endif //USE_FFMPEG
};
//! [0]


#endif /* WINDOW_H_ */
