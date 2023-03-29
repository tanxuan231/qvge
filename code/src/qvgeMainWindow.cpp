/*
This file is a part of
QVGE - Qt Visual Graph Editor

(c) 2016-2018 Ars L. Masiuk (ars.masiuk@gmail.com)

It can be used freely, maintaining the information above.
*/

#include "qvgeMainWindow.h"
#include "qvgeNodeEditorUIController.h"
#include "qvgeVersion.h"

#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QFileInfo>

qvgeMainWindow::qvgeMainWindow()
{
    QApplication::setApplicationName("Road Design Tool");
    QApplication::setApplicationVersion(qvgeVersion.toString() + tr(" (Beta)"));

	CDocumentFormat gexf = { "GEXF", "*.gexf", {"gexf"}, true, true };
	CDocumentFormat graphml = { "GraphML", "*.graphml", {"graphml"}, false, true };
	CDocumentFormat xgr = { "XML Graph", "*.xgr", {"xgr"}, true, true };
    CDocumentFormat gml = { "GML", "*.gml", { "gml" }, false, true };
    CDocument graph = { tr("Graphic"), tr("Directed or undirected graph"), "graph", true,
                        {gexf, graphml, gml, xgr} };
    addDocument(graph);
}


void qvgeMainWindow::init(int argc, char *argv[])
{
    Super::init(argc, argv);

    statusBar()->showMessage(tr("qvge started."));

    createDocument("graph");
}


bool qvgeMainWindow::createDocument(const QByteArray &docType)
{
    // scene
    if (docType == "graph")
    {
		m_graphEditController = new qvgeNodeEditorUIController(this);

        // restore settings for this instance
        readSettings();

        return true;
    }

    // unknown type
    return false;
}


void qvgeMainWindow::onNewDocumentCreated(const QByteArray &docType)
{
	// wizard
	if (docType == "graph")
	{
        m_graphEditController->onNewDocumentCreated();
	}
}


bool qvgeMainWindow::openDocument(const QString &fileName, QByteArray &docType)
{
    return false;
}

QString qvgeMainWindow::getAboutText() const
{
	return Super::getAboutText()
        + QString("<p>The <b>Road Design Tool</b> "
                "It's an intelligent design product for designers.");
}


void qvgeMainWindow::doReadSettings(QSettings& settings)
{
	Super::doReadSettings(settings);

	if (m_graphEditController)
	{
		m_graphEditController->doReadSettings(settings);
	}
}


void qvgeMainWindow::doWriteSettings(QSettings& settings)
{
	Super::doWriteSettings(settings);

	if (m_graphEditController)
	{
		m_graphEditController->doWriteSettings(settings);
	}
}
