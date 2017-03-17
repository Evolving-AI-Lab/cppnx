/*
 * window.h
 *
 *  Created on: May 22, 2013
 *      Author: joost
 */

#ifndef WINDOW_H_
#define WINDOW_H_

//Std includes
#include <iostream>

//Qt includes
#include <QDialog>
#include <QtGui>

//Local includes
#include "CE_LabelWidget.h"
#include "CE_NodeViewWidget.h"
#include "CE_CppnWidget.h"
#include "CX_WeightWidget.h"
#include "CX_FileInformation.h"
#include "CX_GlobalSettings.h"
#include "CE_CppnWriter.h"
#include "CE_CppnParser.h"
#include "CE_MainParser.h"
#include "CX_ComBase.h"
#include "CX_Legend.hpp"
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
    void createNew();
    void load();
    void markDifferences();
    void importPositions();
    void importLabels();
    void askRootDir();
    void loadByID();
    void loadGeneration();
    void nextGeneration();
    void previousGeneration();

    void save();
    void saveAs();

    //Capture and scan actions
    void startScan();
    void stopScan();
    void startFilm();
    void stopFilm();
    void captureFrame();
    void exportToImageSeries();

    //Update actions
    void updateEdge(Edge* edge);
    void updateNode(Node* node);

    //Execute command action
    void executeCommand(ComBase* command);

    //Custom modified slot to circumvent a QT bug (https://bugreports.qt-project.org/browse/QTBUG-20150)
    void onCleanState(bool state);


    void selectAll();
    void remove();
    void exportToJpg();

    /**
     * Enables or disables actions based on the current state of selected items.
     *
     * Currently responsible for:
     * - exportImageAction
     * - removeAction
     * - selectAllAction
     */
    void onSelectionChanged();

    void updateModularity(double qscore);

    void startAnalysis();
    void writeBranchingVsMetric(QDir& directory,
            QString fileName,
            std::map<int, double>& map,
            std::map<int, std::vector<int> >& reverseFamilyMap,
            double correction = 0);
    void generateRandomNetwork(int branch, size_t iteration, QTextStream& stream);
    std::vector<double> readNullModels(QString fileName);

    void createModuleLabels();

    //Mutations
    void addConMut();
    void addNodeMut();

    //Legend
    void toggleLegend();

    //Preferences
    void preferences();

public:
    enum MutationType {addNeuron, addConnection};

    Window();
    virtual ~Window();

    void loadSettings();
    void saveSettings();

    void loadFile(QString filename, int generation = -1);

protected:
    void closeEvent(QCloseEvent * event);

private:
    static const QString modularityText;
    static const QString hierachyText;
    static const QString generationText;

    void _markDifferences(QString filename, int generation = -1);
    void _importPositions(QString filename, int generation = -1);
    void _importLabels(QString filename, int generation = -1);

    bool _checkCanSwicthGeneration();
    QString _getStorageFile(int generation);
    void _loadBranch(int branchNr, int generation = -1);
    void _updateCurrentGeneration(int generation);

    bool askSaveChanges();
    void fileLoaded(bool selected);
    void actualSave(const QString& fileName);

    Edge* _createConMutEdge(Cppn* cppn);
    QPair<QList<Edge*>, Node*> _createNodeMutNodeAndEdges(Cppn* cppn);
    Cppn* _createRandomCppnNeatMutations(size_t nrOfNeurons, size_t nrOfConnections, bool color);
    Cppn* _createRandomCppnFfIoAddCon(size_t nrOfNeurons, size_t nrOfConnections, bool color);
    Cppn* _createRandomCppnFfAddCon(size_t nrOfNeurons, size_t nrOfConnections);
    Cppn* _createInitialCppn(bool color);
    Cppn* _createCppnWithIO(bool color);
    Cppn* _addRandomNodes(Cppn* cppn, size_t nrOfNodes);
    Cppn* _addRandomConnections(Cppn* cppn, size_t nrOfConnections);

    //Analysis functions
    void _dumpCppn(Cppn* cppn, int branch, size_t iteration, QTextStream& stream);

    //Create action
    QAction* _createAction(QString name, QString statusBarTip, const char *member);

    QString _formatWeight(double weight);

    //The undo stack
    QUndoStack undoStack;

    //Menu objects
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu* editMenu;
    QMenu* posMenu;
    QMenu* viewMenu;
    QMenu* _experimentalMenu;
    QMenu* _searchMenu;

    //Child widgets
    NodeViewWidget* nodeviewWidget;
    LabelWidget* labelWidget;
    CppnWidget *cppnWidget;
    WeightWidget* weightWidget;
    FileInformation* fileInformation;

    //Actions
    QAction *exitAction;
    QAction *_newAction;
    QAction *loadAction;
    QAction *loadIDAction;
    QAction *_nextGeneration;
    QAction *_previousGeneration;
    QAction *_startAnalysisAction;
    QAction *_compareGenomesAction;
    QAction *_importPositionsAction;
    QAction *_importLabelsAction;
    QAction *_createModuleLabelsAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *selectAllAction;
    QAction *removeAction;
    QAction *exportImageAction;
    QAction *_exportToImageSeriesAction;
    QAction *_toggleLegendAction;
    QAction *_preferencesAction;

    //Layout objects
    QHBoxLayout *mainLayout;
    QVBoxLayout* leftLayout;

    //Capture variables
    QString captureDirectory;
    QString extention;
    QList<NodeView*> nodeViewsToBeCaptured;
    size_t frame;

    //Genome dir
//QString _genomeDir;
    int _currentBranch;
    int _currentGeneration;
    int _maxGeneration;
    int _parent;
    std::map<int, int> _storageMap;
    std::vector<int> _branchHistory;

//    QStatusBar* bar;

    QLabel* modularityLabel;
    QLabel* hierachyLabel;

    double _qscore;

    //QSettings
    QString _settingsFile;

#ifdef USE_FFMPEG
    QList<QVideoEncoder*> nodeViewEncoders;
    QVideoEncoder encoder;
#endif //USE_FFMPEG
};

#endif /* WINDOW_H_ */
