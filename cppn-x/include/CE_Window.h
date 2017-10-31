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

	//////////////////////////////////////////////////////////////////
	// User actions:                                                //
	// These slots are tied to menu actions that a user can execute //
	//////////////////////////////////////////////////////////////////

	/**
	 * Creates a new cppn from scratch
	 */
    void createNew();

    /**
     * Loads a cppn from a file.
     */
    void load();

    /**
     * Marks the differences between this cppn and another cppn.
     */
    void markDifferences();

    /**
     * Moves all nodes in this cppn that are also in a loaded cppn to the same
     * absolute positions.
     */
    void importPositions();

    /**
     * Imports all labels of the other cppn.
     *
     * Will apply the imported labels to any nodes or connections that the files
     * have in common.
     */
    void importLabels();

    /**
     * Creates a label for every detected module.
     *
     * This can be useful if you want to change the colors of the automatically
     * detected modules.
     */
    void createModuleLabels();

    /**
     * Loads a cppn by ID, provided that a cppn archive is available.
     */
    void loadByID();

    /**
     * Loads a specific generation for the current cppn.
     */
    void loadGeneration();

    /**
     * Loads the next generation.
     */
    void nextGeneration();

    /**
     * Loads the previous generation.
     */
    void previousGeneration();

    /**
     * Saves the current cppn to the current location.
     */
    void save();

    /**
     * Saves the current cppn to an indicated location.
     */
    void saveAs();

    /**
     * Starts a scan.
     */
    void startScan();

    /**
     * Stops the scan.
     */
    void stopScan();

    /**
     * Starts a scan and saves every intermediate frame to a file.
     */
    void startFilm();

    /**
     * Stops the film.
     */
    void stopFilm();

    /**
     * Creates a film for every connection in the network.
     */
    void filmEveryConnection();

    /**
     * Similar to start film, but always stores images, rather than creating
     * a movie.
     */
    void exportToImageSeries();

    /**
     * Exports the image of whatever node is currently selected to a file.
     */
    void exportToJpg();

    /**
     * Selects all items in the widget that currently has the focus.
     */
    void selectAll();

    /**
     * Removes whatever item is selected in the widget that has focus.
     */
    void remove();

    /**
     * Adds or remove the legend.
     */
    void toggleLegend();

    /**
     * Shows the preferences menu.
     */
    void preferences();

    /**
     * Starts an analysis of all cppns in our genome directory.
     *
     * This action is hidden is most release versions of CPPN-X
     */
    void startAnalysis();

    /**
     * Applies an add-connection mutation to the current network.
     *
     * TODO: Should be a function of the cppn widget, not the main window.
     */
    void addConMut();

    /**
     * Applies an add-mode mutation to the current network.
     *
     * TODO: Should be a function of the cppn widget, not the main window.
     */
    void addNodeMut();


    /**
     * Generate a null model.
     */
    void createNullModel();

	//////////////////////////////////////////////////////////////////
	// Internal slots:                                              //
	// These slots are tied to other widgets, but can not be        //
    // directly executed by the user
	//////////////////////////////////////////////////////////////////

//    // Update actions
//    void updateEdge(Edge* edge);
//    void updateNode(Node* node);

    /**
     * Executes the provided command.
     *
     * This functions is tied to the main window because the main window keeps
     * track of the undo stack.
     */
    void executeCommand(ComBase* command);

    /**
     * Custom modified slot to circumvent a QT bug
     *
     * (https://bugreports.qt-project.org/browse/QTBUG-20150)
     */
    void onCleanState(bool state);

    /**
     * Enables or disables actions based on the current state of selected items.
     *
     * Currently responsible for:
     * - exportImageAction
     * - removeAction
     * - selectAllAction
     */
    void onSelectionChanged();

    /**
     * Captures a single frame for the film.
     */
    void captureFrame();

    /**
     * Updates the modularity value shown in the status bar.
     */
    void updateModularity(double qscore);

    /**
     * Updates the generation shown in the status bar.
     */
    void updateGeneration(int generation);

public:
    enum MutationType {addNeuron, addConnection};

    /**
     * Constructs a main window.
     */
    Window();

    /**
     * Destroys the main window.
     */
    virtual ~Window();

    /**
     * Actually loads the indicated file.
     *
     * Load file is public, rather than private, because it allows main to
     * automatically load a file when, for example, choosing the "open with"
     * option.
     */
    void loadFile(QString filename, int generation = -1);

protected:
    void closeEvent(QCloseEvent * event);

private:
    static const QString modularityText;
    static const QString hierachyText;
    static const QString generationText;
    static const QString branchText;
    static const QString url;

    /**
     * Load preferences from disk.
     */
    void loadSettings();

    /**
     * Saves preferences to disk.
     */
    void saveSettings();



    /**
     * Asks the user for the location of the genome directory.
     */
    void askRootDir();

    /**
     * Actually marks the differences between this cppn and the indicated file.
     */
    void _markDifferences(QString filename, int generation = -1);

    /**
     * Actually imports the positions from the indicated file.
     */
    void _importPositions(QString filename, int generation = -1);

    /**
     * Actually imports the labels from the indicated file.
     */
    void _importLabels(QString filename, int generation = -1);

    /**
     * Checks whether we can switch generations, which requires the current
     * cppn to be loaded through the genome archive.
     */
    bool _checkCanSwitchGeneration();

    /**
     * Constructs the name of the storage file for the indicated generation.
     */
    QString _getStorageFile(int generation);

    /**
     * Constructs the name of the main file for the indiacted branch.
     */
    QString _getMainFile(int branchNr);

    /**
     * Loads the indicated branch.
     */
    void _loadBranch(int branchNr, int generation = -1);

    /**
     * Updates the status text after parts of it have been changed.
     */
    void _updateStatusText();

    /**
     * Asks the user whether he/she wants to save the changes made.
     */
    bool askSaveChanges();

    /**
     * Registers that a file has been loaded, enabling the appropriate options.
     */
    void fileLoaded(bool selected);

    /**
     * Actually save the current cppn to the indicated file.
     */
    void actualSave(const QString& fileName);

    /**
     * Convenience function for quickly creating actions.
     */
    QAction* _createAction(QString name, QString tip, const char *member);

    /**
     * Formats a weight into a form appropriate for a file name.
     */
    QString _formatWeight(double weight);

    /**
     * Returns a list of all branches in the genome dir folder.
     */
    QList<QString> _getAvailableBranches();


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
    QAction *_nullModelAction;
    QAction *_filmEveryConnectionAction;

    //Layout objects
    QHBoxLayout *mainLayout;
    QVBoxLayout* leftLayout;

    //Capture variables
    QString captureDirectory;
    QString extention;
    QList<NodeView*> nodeViewsToBeCaptured;
    size_t frame;

    //Genome dir
    int _currentBranch;
    int _currentGeneration;
    int _maxGeneration;
    int _parent;
    std::map<int, int> _storageMap;
    std::vector<int> _branchHistory;

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
