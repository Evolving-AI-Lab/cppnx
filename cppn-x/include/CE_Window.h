/*
 * window.h
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */

#ifndef WINDOW_H_
#define WINDOW_H_

//Qt includes
#include <QDialog>
#include <QtGui>

//Std includes
#include <iostream>

//Local includes
#include "CE_LabelWidget.h"
#include "CE_NodeViewWidget.h"
#include "CE_CppnWidget.h"
#include "CX_WeightWidget.h"
#include "CX_FileInformation.h"
#include "CE_CppnWriter.h"
#include "CE_CppnParser.h"
#include "CX_ComBase.h"
#ifdef USE_FFMPEG
#include "QVideoEncoder.h"
#endif //USE_FFMPEG


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMenuBar;
QT_END_NAMESPACE

class Window : public QMainWindow
{
    Q_OBJECT
public slots:
	//Save load actions
    void load();
    void save();
    void saveAs();

    //Capture and scan actions
    void startScan();
    void stopScan();
    void startFilm();
    void stopFilm();
    void captureFrame();

    //Update actions
    void updateEdge(Edge* edge);
    void updateNode(Node* node);

    //Execute command action
    void executeCommand(ComBase* command);

    //Custom modified slot to circumvent a QT bug (https://bugreports.qt-project.org/browse/QTBUG-20150)
    void onSceneModified();
    void onCleanState(bool state);


    void selectAll();
    void remove();
    void exportToJpg();

    void onSelectionChanged();

    void updateModularity(double qscore);

public:
    Window();
    virtual ~Window();

protected:
    void closeEvent(QCloseEvent * event);

private:
    static const QString modularityText;
    static const QString hierachyText;

    bool askSaveChanges();
    void fileLoaded(bool selected);
    void actualSave(const QString& fileName);

    //The undo stack
    QUndoStack undoStack;

    //Menu objects
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu* editMenu;
    QMenu* posMenu;
    QMenu* viewMenu;

    //Child widgets
    NodeViewWidget* nodeviewWidget;
    LabelWidget* labelWidget;
    CppnWidget *cppnWidget;
    WeightWidget* weightWidget;
    FileInformation* fileInformation;

    //Actions
    QAction *exitAction;
    QAction *loadAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *selectAllAction;
    QAction *removeAction;
    QAction *exportImageAction;

    //Layout objects
    QHBoxLayout *mainLayout;
    QVBoxLayout* leftLayout;

    //Capture variables
    QString captureDirectory;
    QString extention;
    QList<NodeView*> nodeViewsToBeCaptured;
    size_t frame;

//    QStatusBar* bar;

    QLabel* modularityLabel;
    QLabel* hierachyLabel;


#ifdef USE_FFMPEG
    QList<QVideoEncoder*> nodeViewEncoders;
    QVideoEncoder encoder;
#endif //USE_FFMPEG
};

#endif /* WINDOW_H_ */
