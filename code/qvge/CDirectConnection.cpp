/*
This file is a part of
QVGE - Qt Visual Graph Editor

(c) 2016-2018 Ars L. Masiuk (ars.masiuk@gmail.com)

It can be used freely, maintaining the information above.
*/

#include <QDebug>

#include "CDirectConnection.h"
#include "CNode.h"


CDirectConnection::CDirectConnection(QGraphicsItem *parent): Super(parent)
{

}

// reimp

CConnection* CDirectConnection::clone()
{
	CDirectConnection* c = new CDirectConnection(parentItem());

	//c->setFirstNode(m_firstNode);
	//c->setLastNode(m_lastNode);

	// assign directly!
	c->m_firstNode = m_firstNode;
	c->m_lastNode = m_lastNode;

	if (scene())
		scene()->addItem(c);

	c->copyDataFrom(this);

	return c;
}


void CDirectConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget)
{
	//qDebug() << boundingRect() << option->exposedRect << option->rect;

	// called before draw 
    setupPainter(painter, option, widget);

    painter->setClipRect(boundingRect());
    painter->drawLine(line());
}

// callbacks 

void CDirectConnection::onParentGeometryChanged()
{
	if (!m_firstNode || !m_lastNode)
		return;

	// optimize: no update while restoring
	if (s_duringRestore)
		return;

	prepareGeometryChange();
//    qDebug()<<__FUNCTION__;
	// update line position
	QPointF p1 = m_firstNode->pos(), p2 = m_lastNode->pos();
	QLineF l(p1, p2);
    setLine(l);

//	// update shape path
    QPainterPath path;
    path.moveTo(p1);
    // center
    m_controlPos = (p1 + p2) / 2;
    path.lineTo(p2);

    QPainterPathStroker stroker;    // 对path进行描边操作
    stroker.setWidth(6);
    m_selectionShapePath = stroker.createStroke(path);

    update();
}

