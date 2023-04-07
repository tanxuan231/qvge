/*
This file is a part of
QVGE - Qt Visual Graph Editor

(c) 2016-2018 Ars L. Masiuk (ars.masiuk@gmail.com)

It can be used freely, maintaining the information above.
*/

#pragma once

#include "CEditorScene.h"
#include "base/ConfigFileOper.h"

class CNode;
class CConnection;


enum EditMode 
{
	EM_Default,
	EM_AddNodes,
	EM_AddEdges
};


class CNodeEditorScene : public CEditorScene
{
	Q_OBJECT

public:
	typedef CEditorScene Super;

	CNodeEditorScene(QObject *parent);

	// reimp
	virtual void initialize();
	virtual void initializeOnce();

	// operations
	bool startNewConnection(const QPointF& pos);
	void cancel(const QPointF& pos = QPointF());

	EditMode getEditMode() const {
		return m_editMode;
	}

	// factorizations
	virtual CNode* createNewNode() const;
	virtual CConnection* createNewConnection() const;

	CConnection* activateConnectionFactory(const QByteArray& factoryId);

    // selections
    virtual void moveSelectedItemsBy(const QPointF& d);

    const QList<CNode*>& getSelectedNodes();
    const QList<CConnection*>& getSelectedEdges();

    CNode* AddNewNode(const QPointF& point,
                      bool selected = false,
                      NodeLabel label = NodeLabel::FreePoint,
                      bool unredo = true);
    void AddNewConnection(CNode* start, CNode* end, bool unredo = true);
    void DrawPolygon(const QPolygonF& polygon, const QPen& pen, bool isMovable = false);

Q_SIGNALS:
	void editModeChanged(int mode);

public Q_SLOTS:
	virtual void onActionLink();
	virtual void onActionUnlink();
	virtual void onActionNodeColor();
	virtual void onActionEdgeColor();
	void onActionEdgeReverse();
	void onActionEdgeDirected();
	void onActionEdgeMutual();
	void onActionEdgeUndirected();

    void onSceneOrSelectionChanged();

	void setEditMode(EditMode mode);

protected:
    void prefetchSelection();

	// scene events
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void keyPressEvent(QKeyEvent *keyEvent);

	// called on drag after single click; returns true if handled
	virtual bool onClickDrag(QGraphicsSceneMouseEvent *mouseEvent, const QPointF &clickPos);

	virtual void onDropped(QGraphicsSceneMouseEvent* mouseEvent, QGraphicsItem* dragItem);
	virtual void onLeftClick(QGraphicsSceneMouseEvent* mouseEvent, QGraphicsItem* clickedItem);    

	// reimp
	virtual bool populateMenu(QMenu& menu, QGraphicsItem* item, const QList<QGraphicsItem*>& selectedItems);
	virtual QList<QGraphicsItem*> copyPasteItems() const;

    // draw
    virtual void drawBackground(QPainter *painter, const QRectF &);
    virtual void drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[],
                           QWidget *widget = Q_NULLPTR);
protected:
	// edit mode
	EditMode m_editMode;

	// creating
	CNode *m_startNode, *m_endNode;
	CConnection *m_connection;
    std::vector<CConnection*> m_connections;
	bool m_realStart;

	enum InternState {
		IS_None, IS_Creating, IS_Finishing, IS_Cancelling
	};
	InternState m_state;

    // cached selections
    QList<CNode*> m_selNodes;
    QList<CConnection*> m_selEdges;

    // drawing
    int m_nextIndex = 0;
};


