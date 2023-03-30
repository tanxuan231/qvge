#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QPointF>
#include <QLineF>
#include <QList>
#include <QPolygonF>
#include <QString>

#define BUILDINGS_STR "buildings"
#define BRODERS_STR "border"

enum class NodeLabel {
    // 0 出入口；1 自由端点；2 发生延拓碰到shell的端点；3 发生延拓碰到建筑/障碍物的端点；4 交点；5 拐点
    Unknown = -1,
    Entrance,
    FreePoint,
    TouchShellPoint,
    TouchObstaclePoint,
    Intersection,
    Inflection,
};

using EdgeInfo = std::vector<std::pair<int, int>>;
using NodeInfo = std::vector<QPointF>;
using NodeLabelInfo = std::vector<NodeLabel>;

class ConfigReader
{
public:
    using PolygonList = QList<QPolygonF>;
    explicit ConfigReader(const QString& filePath);

    bool Init();

    PolygonList GetBroders() const
    {
        return m_broderPolygons;
    }

    PolygonList GetBuildings() const
    {
        return m_buildingPolygons;
    }

    PolygonList GetClimbings() const
    {
        return m_climbingPolygons;
    }

    PolygonList GetEntries() const
    {
        return m_entriesPolygons;
    }

    PolygonList GetTurningSpace() const
    {
        return m_turningSpacePolygons;
    }

    PolygonList GetObstacles() const
    {
        return m_obstaclePolygons;
    }

    EdgeInfo GetRoadEdges() const
    {
        return m_roadEdges;
    }

    NodeInfo GetRoadNodes() const
    {
        return m_roadNodes;
    }

    NodeLabelInfo GetRoadNodeLables() const
    {
        return m_roadNodeLabels;
    }

private:
    struct PointsRelation {
        QPointF pre;
        QPointF cur;
        QPointF next;
    };

    void ReadPoints(const QJsonObject &jsonObj, QPolygonF &polygon);
private:
    QString m_confFilePath;

    PolygonList m_broderPolygons;
    PolygonList m_buildingPolygons;
    PolygonList m_climbingPolygons;
    PolygonList m_entriesPolygons;
    PolygonList m_turningSpacePolygons;
    PolygonList m_obstaclePolygons;

    EdgeInfo m_roadEdges;
    NodeInfo m_roadNodes;
    NodeLabelInfo m_roadNodeLabels;
};


#endif // CONFIGREADER_H

