/*
This file is a part of
QVGE - Qt Visual Graph Editor

(c) 2016-2018 Ars L. Masiuk (ars.masiuk@gmail.com)

It can be used freely, maintaining the information above.
*/

#pragma once

#include "base/ConfigFileOper.h"
#include <QAction>
#include <QLabel>
#include <QSettings>


class qvgeMainWindow;

class CNodeEditorScene;
class CEditorView;
class IFileSerializer;


class qvgeNodeEditorUIController : public QObject
{
	Q_OBJECT

public:
	qvgeNodeEditorUIController(qvgeMainWindow *parent);
	~qvgeNodeEditorUIController();

    void LoadConfigure();
    void LoadRoad(const EdgeInfo &edges,
                  const NodeInfo &nodes,
                  const NodeLabelInfo &labels);
    void LoadClimbingSpace(const QList<QPolygonF>& polygons, const QPen& pen);
    void LoadTuringSpace(const QList<QPolygonF>& polygons, const QPen& pen);
    void LoadPolygon(const QList<QPolygonF>& polygons, const QPen& pen);

	void doReadSettings(QSettings& settings);
	void doWriteSettings(QSettings& settings);

    void onNewDocumentCreated();

private Q_SLOTS:	
	void exportFile();

	void onSelectionChanged();
    void onSceneChanged();
	void onSceneHint(const QString& text);
	void onSceneStatusChanged(int status);
	void sceneEditMode(QAction*);
	void onEditModeChanged(int mode);

	void onZoomChanged(double currentZoom);
	void zoom();
	void unzoom();
	void resetZoom();

	void sceneCrop();
    void sceneOptions();

private:
	void createMenus();

private:
	qvgeMainWindow *m_parent;
	CNodeEditorScene *m_editorScene;
	CEditorView *m_editorView;

    QLabel *m_statusLabel;

	QString m_lastExportPath;

	QAction *cutAction;
	QAction *copyAction;
	QAction *pasteAction;
	QAction *delAction;
	QAction *linkAction;
	QAction *unlinkAction;

	QActionGroup *m_editModesGroup;
	QAction *modeDefaultAction;
	QAction *modeNodesAction;
	QAction *modeEdgesAction;

	QAction *zoomAction;
	QAction *unzoomAction;
	QAction *resetZoomAction;
	QAction *resetZoomAction2;
	QAction *fitZoomAction;

    QAction *gridAction;
    QAction *gridSnapAction;
    QAction *actionShowLabels;

	bool m_showNewGraphDialog = true;
};
