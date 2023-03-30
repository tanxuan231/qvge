/*
This file is a part of
QVGE - Qt Visual Graph Editor

(c) 2016-2018 Ars L. Masiuk (ars.masiuk@gmail.com)

It can be used freely, maintaining the information above.
*/

#include <qvgeNodeEditorUIController.h>
#include <qvgeMainWindow.h>

#include <qvge/CNode.h>
#include <qvge/CConnection.h>
#include <qvge/CNodeEditorScene.h>
#include <qvge/CEditorSceneDefines.h>
#include <qvge/CEditorView.h>

#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QMenu>
#include <QToolButton>
#include <QWidgetAction>
#include <QResizeEvent>
#include <QDebug>
#include <QPixmapCache>
#include <QFileDialog>


qvgeNodeEditorUIController::qvgeNodeEditorUIController(qvgeMainWindow *parent) :
	QObject(parent),
	m_parent(parent)
{
	// create document
	m_editorScene = new CNodeEditorScene(parent);
	m_editorView = new CEditorView(m_editorScene, parent);
	parent->setCentralWidget(m_editorView);

	// connect scene
	connect(m_editorScene, &CEditorScene::sceneChanged, parent, &CMainWindow::onDocumentChanged);
    connect(m_editorScene, &CEditorScene::sceneChanged, this, &qvgeNodeEditorUIController::onSceneChanged);
	connect(m_editorScene, &CEditorScene::selectionChanged, this, &qvgeNodeEditorUIController::onSelectionChanged);

	connect(m_editorScene, &CEditorScene::infoStatusChanged, this, &qvgeNodeEditorUIController::onSceneStatusChanged);
	connect(m_editorScene, &CNodeEditorScene::editModeChanged, this, &qvgeNodeEditorUIController::onEditModeChanged);

	// connect view
	connect(m_editorView, SIGNAL(scaleChanged(double)), this, SLOT(onZoomChanged(double)));

	// menus & actions
	createMenus();

    // status bar
    m_statusLabel = new QLabel();
    parent->statusBar()->addPermanentWidget(m_statusLabel);

    // update actions
    onSceneChanged();
    onSelectionChanged();
    onZoomChanged(1);
	onSceneStatusChanged(m_editorScene->getInfoStatus());


    auto start = m_editorScene->AddNewNode(QPointF(100, 100));
    auto end = m_editorScene->AddNewNode(QPointF(50, 200));
    m_editorScene->AddNewConnection(start, end);
}


void qvgeNodeEditorUIController::createMenus()
{
	// file actions
//	QAction *exportAction = m_parent->getFileExportAction();
//	exportAction->setVisible(true);
//	exportAction->setText(tr("Export to &Image..."));
//	connect(exportAction, &QAction::triggered, this, &qvgeNodeEditorUIController::exportFile);

	// add edit menu
	QMenu *editMenu = new QMenu(tr("&Edit"));
	m_parent->menuBar()->insertMenu(m_parent->getWindowMenuAction(), editMenu);

	QAction *undoAction = editMenu->addAction(QIcon(":/Icons/Undo"), tr("&Undo"));
	undoAction->setStatusTip(tr("Undo latest action"));
	undoAction->setShortcut(QKeySequence::Undo);
	connect(undoAction, &QAction::triggered, m_editorScene, &CEditorScene::undo);
	connect(m_editorScene, &CEditorScene::undoAvailable, undoAction, &QAction::setEnabled);
	undoAction->setEnabled(m_editorScene->availableUndoCount());

	QAction *redoAction = editMenu->addAction(QIcon(":/Icons/Redo"), tr("&Redo"));
	redoAction->setStatusTip(tr("Redo latest action"));
	redoAction->setShortcut(QKeySequence::Redo);
	connect(redoAction, &QAction::triggered, m_editorScene, &CEditorScene::redo);
	connect(m_editorScene, &CEditorScene::redoAvailable, redoAction, &QAction::setEnabled);
	redoAction->setEnabled(m_editorScene->availableRedoCount());

	editMenu->addSeparator();

	cutAction = editMenu->addAction(QIcon(":/Icons/Cut"), tr("Cu&t"));
	cutAction->setStatusTip(tr("Cut selection to clipboard"));
	cutAction->setShortcut(QKeySequence::Cut);
	connect(cutAction, &QAction::triggered, m_editorScene, &CEditorScene::cut);

	copyAction = editMenu->addAction(QIcon(":/Icons/Copy"), tr("&Copy"));
	copyAction->setStatusTip(tr("Copy selection to clipboard"));
	copyAction->setShortcut(QKeySequence::Copy);
	connect(copyAction, &QAction::triggered, m_editorScene, &CEditorScene::copy);

	pasteAction = editMenu->addAction(QIcon(":/Icons/Paste"), tr("&Paste"));
	pasteAction->setStatusTip(tr("Paste selection from clipboard"));
	pasteAction->setShortcut(QKeySequence::Paste);
	connect(pasteAction, &QAction::triggered, m_editorScene, &CEditorScene::paste);

	delAction = editMenu->addAction(QIcon(":/Icons/Delete"), tr("&Delete"));
	delAction->setStatusTip(tr("Delete selection"));
	delAction->setShortcut(QKeySequence::Delete);
	connect(delAction, &QAction::triggered, m_editorScene, &CEditorScene::del);

	editMenu->addSeparator();

	linkAction = editMenu->addAction(QIcon(":/Icons/Link"), tr("&Link"));
	linkAction->setStatusTip(tr("Link selected nodes together"));
	connect(linkAction, &QAction::triggered, m_editorScene, &CNodeEditorScene::onActionLink);

	unlinkAction = editMenu->addAction(QIcon(":/Icons/Unlink"), tr("&Unlink"));
	unlinkAction->setStatusTip(tr("Unlink selected nodes"));
	connect(unlinkAction, &QAction::triggered, m_editorScene, &CNodeEditorScene::onActionUnlink);


	// edit modes
	editMenu->addSeparator();

	m_editModesGroup = new QActionGroup(this);
	m_editModesGroup->setExclusive(true);
	connect(m_editModesGroup, &QActionGroup::triggered, this, &qvgeNodeEditorUIController::sceneEditMode);

	modeDefaultAction = editMenu->addAction(QIcon(":/Icons/Mode-Select"), tr("Select Items"));
	modeDefaultAction->setToolTip(tr("Items selection mode"));
	modeDefaultAction->setStatusTip(tr("Select/deselect items in the document"));
	modeDefaultAction->setCheckable(true);
	modeDefaultAction->setActionGroup(m_editModesGroup);
	modeDefaultAction->setChecked(m_editorScene->getEditMode() == EM_Default);
	modeDefaultAction->setData(EM_Default);

	modeNodesAction = editMenu->addAction(QIcon(":/Icons/Mode-AddNodes"), tr("Create Nodes"));
	modeNodesAction->setToolTip(tr("Adding new nodes mode"));
	modeNodesAction->setStatusTip(tr("Quickly add nodes & edges"));
	modeNodesAction->setCheckable(true);
	modeNodesAction->setActionGroup(m_editModesGroup);
	modeNodesAction->setChecked(m_editorScene->getEditMode() == EM_AddNodes);
	modeNodesAction->setData(EM_AddNodes);

	//modeEdgesAction = editMenu->addAction(tr("Add edges mode"));
	//modeEdgesAction->setCheckable(true);
	//modeEdgesAction->setActionGroup(m_editModesGroup);
	//modeEdgesAction->setChecked(m_editorScene->getEditMode() == EM_AddEdges);
	//modeEdgesAction->setData(EM_AddEdges);


	// scene actions
	editMenu->addSeparator();

	QAction *sceneCropAction = editMenu->addAction(QIcon(":/Icons/Crop"), tr("&Crop Area"));
	sceneCropAction->setStatusTip(tr("Crop document area to contents"));
	connect(sceneCropAction, &QAction::triggered, this, &qvgeNodeEditorUIController::sceneCrop);

	// scene options
	editMenu->addSeparator();

	// add edit toolbar
	QToolBar *editToolbar = m_parent->addToolBar(tr("Edit"));
    editToolbar->setObjectName("editToolbar");
	editToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	editToolbar->addAction(undoAction);
	editToolbar->addAction(redoAction);

	editToolbar->addSeparator();

	editToolbar->addAction(cutAction);
	editToolbar->addAction(copyAction);
	editToolbar->addAction(pasteAction);
	editToolbar->addAction(delAction);

	editToolbar->addSeparator();

	// add edit modes toolbar
	QToolBar *editModesToolbar = m_parent->addToolBar(tr("Edit Modes"));
	editModesToolbar->setObjectName("editModesToolbar");
	editModesToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	editModesToolbar->addAction(modeDefaultAction);
	editModesToolbar->addAction(modeNodesAction);


	// add view menu
	QMenu *viewMenu = new QMenu(tr("&View"));
	m_parent->menuBar()->insertMenu(m_parent->getWindowMenuAction(), viewMenu);

//    gridAction = viewMenu->addAction(QIcon(":/Icons/Grid-Show"), tr("Show &Grid"));
//	gridAction->setCheckable(true);
//	gridAction->setStatusTip(tr("Show/hide background grid"));
//	gridAction->setChecked(m_editorScene->gridEnabled());
//	connect(gridAction, SIGNAL(toggled(bool)), m_editorScene, SLOT(enableGrid(bool)));

//    gridSnapAction = viewMenu->addAction(QIcon(":/Icons/Grid-Snap"), tr("&Snap to Grid"));
//	gridSnapAction->setCheckable(true);
//	gridSnapAction->setStatusTip(tr("Snap to grid when dragging"));
//	gridSnapAction->setChecked(m_editorScene->gridSnapEnabled());
//	connect(gridSnapAction, SIGNAL(toggled(bool)), m_editorScene, SLOT(enableGridSnap(bool)));

//    actionShowLabels = viewMenu->addAction(QIcon(":/Icons/Label"), tr("Show &Labels"));
//	actionShowLabels->setCheckable(true);
//	actionShowLabels->setStatusTip(tr("Show/hide item labels"));
//	actionShowLabels->setChecked(m_editorScene->itemLabelsEnabled());
//	connect(actionShowLabels, SIGNAL(toggled(bool)), m_editorScene, SLOT(enableItemLabels(bool)));

//	viewMenu->addSeparator();

	zoomAction = viewMenu->addAction(QIcon(":/Icons/ZoomIn"), tr("&Zoom"));
	zoomAction->setStatusTip(tr("Zoom view in"));
	zoomAction->setShortcut(QKeySequence::ZoomIn);
	connect(zoomAction, &QAction::triggered, this, &qvgeNodeEditorUIController::zoom);

	unzoomAction = viewMenu->addAction(QIcon(":/Icons/ZoomOut"), tr("&Unzoom"));
	unzoomAction->setStatusTip(tr("Zoom view out"));
	unzoomAction->setShortcut(QKeySequence::ZoomOut);
	connect(unzoomAction, &QAction::triggered, this, &qvgeNodeEditorUIController::unzoom);

	resetZoomAction = viewMenu->addAction(QIcon(":/Icons/ZoomReset"), tr("&Reset Zoom"));
	resetZoomAction->setStatusTip(tr("Zoom view to 100%"));
	connect(resetZoomAction, &QAction::triggered, this, &qvgeNodeEditorUIController::resetZoom);

	fitZoomAction = viewMenu->addAction(QIcon(":/Icons/ZoomFit"), tr("&Fit to View"));
	fitZoomAction->setStatusTip(tr("Zoom to fit all the items to view"));
	connect(fitZoomAction, &QAction::triggered, m_editorView, &CEditorView::fitToView);


	// add view toolbar
	QToolBar *zoomToolbar = m_parent->addToolBar(tr("View"));
    zoomToolbar->setObjectName("viewToolbar");
	zoomToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	zoomToolbar->addAction(zoomAction);

	resetZoomAction2 = zoomToolbar->addAction(QIcon(":/Icons/Zoom"), "");
	resetZoomAction2->setStatusTip(resetZoomAction->statusTip());
	resetZoomAction2->setToolTip(resetZoomAction->statusTip());
	connect(resetZoomAction2, &QAction::triggered, this, &qvgeNodeEditorUIController::resetZoom);

	zoomToolbar->addAction(unzoomAction);
	zoomToolbar->addAction(fitZoomAction);
}

qvgeNodeEditorUIController::~qvgeNodeEditorUIController() 
{
}


void qvgeNodeEditorUIController::onSelectionChanged()
{
	int selectionCount = m_editorScene->selectedItems().size();

	cutAction->setEnabled(selectionCount > 0);
	copyAction->setEnabled(selectionCount > 0);
	delAction->setEnabled(selectionCount > 0);

	auto nodes = m_editorScene->getSelectedItems<CNode>();
	linkAction->setEnabled(nodes.size() > 1);
	unlinkAction->setEnabled(nodes.size() > 0);
}


void qvgeNodeEditorUIController::onSceneChanged()
{
    auto nodes = m_editorScene->getItems<CNode>();
    auto edges = m_editorScene->getItems<CConnection>();

    m_statusLabel->setText(tr("Nodes: %1 | Edges: %2").arg(nodes.size()).arg(edges.size()));
}


void qvgeNodeEditorUIController::onSceneHint(const QString& text)
{
	m_parent->statusBar()->showMessage(text);
}


void qvgeNodeEditorUIController::onSceneStatusChanged(int status)
{
	switch (status)
	{
	case SIS_Hover:
		onSceneHint(tr("Ctrl+Click - (un)select item | Click & drag - move selected items | Ctrl+Click & drag - clone selected items"));
		return;
	case SIS_Drag:
		onSceneHint(tr("Shift - horizontal or vertical snap | Alt - toggle grid snap"));
		return;
	default:
		onSceneHint(tr("Click & drag - select an area"));
	}
}


void qvgeNodeEditorUIController::onZoomChanged(double currentZoom)
{
	resetZoomAction2->setText(QString("%1%").arg((int)(currentZoom * 100)));
}


void qvgeNodeEditorUIController::zoom()
{
	m_editorView->zoomBy(1.3);
}


void qvgeNodeEditorUIController::unzoom()
{
	m_editorView->zoomBy(1.0 / 1.3);
}


void qvgeNodeEditorUIController::resetZoom()
{
	m_editorView->zoomTo(1.0);
}


void qvgeNodeEditorUIController::sceneCrop()
{
	QRectF itemsRect = m_editorScene->itemsBoundingRect().adjusted(-20, -20, 20, 20);
	if (itemsRect == m_editorScene->sceneRect())
		return;

	// update scene rect
	m_editorScene->setSceneRect(itemsRect);

	m_editorScene->addUndoState();
}


void qvgeNodeEditorUIController::sceneOptions()
{
//    CSceneOptionsDialog dialog;
//	dialog.setShowNewGraphDialog(m_showNewGraphDialog);

//    if (dialog.exec(*m_editorScene, *m_editorView))
//    {
//        gridAction->setChecked(m_editorScene->gridEnabled());
//        gridSnapAction->setChecked(m_editorScene->gridSnapEnabled());
//        actionShowLabels->setChecked(m_editorScene->itemLabelsEnabled());

//		m_showNewGraphDialog  = dialog.isShowNewGraphDialog();

//		m_parent->writeSettings();
//    }
}


void qvgeNodeEditorUIController::sceneEditMode(QAction* act)
{
	int mode = act->data().toInt();
	m_editorScene->setEditMode((EditMode)mode);
}


void qvgeNodeEditorUIController::onEditModeChanged(int mode)
{
	if (mode == EM_AddNodes)
		modeNodesAction->setChecked(true);
	else
		modeDefaultAction->setChecked(true);
}

void qvgeNodeEditorUIController::exportFile()
{
}

void qvgeNodeEditorUIController::doReadSettings(QSettings& settings)
{
	bool isAA = m_editorView->renderHints().testFlag(QPainter::Antialiasing);
	isAA = settings.value("antialiasing", isAA).toBool();
	m_editorView->setRenderHint(QPainter::Antialiasing, isAA);
	m_editorScene->setFontAntialiased(isAA);

	int cacheRam = QPixmapCache::cacheLimit();
	cacheRam = settings.value("cacheRam", cacheRam).toInt();
	QPixmapCache::setCacheLimit(cacheRam);

	m_lastExportPath = settings.value("lastExportPath", m_lastExportPath).toString();
	m_showNewGraphDialog  = settings.value("autoCreateGraphDialog", m_showNewGraphDialog ).toBool();
}

void qvgeNodeEditorUIController::doWriteSettings(QSettings& settings)
{
	bool isAA = m_editorView->renderHints().testFlag(QPainter::Antialiasing);
	settings.setValue("antialiasing", isAA);

	int cacheRam = QPixmapCache::cacheLimit();
	settings.setValue("cacheRam", cacheRam);

	settings.setValue("lastExportPath", m_lastExportPath);
	settings.setValue("autoCreateGraphDialog", m_showNewGraphDialog );
}

void qvgeNodeEditorUIController::onNewDocumentCreated()
{
	m_editorScene->setClassAttributeVisible("item", "id", true);
	m_editorScene->setClassAttributeVisible("item", "label", true);
}
