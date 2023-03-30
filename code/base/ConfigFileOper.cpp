#include "ConfigFileOper.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <unordered_map>

ConfigReader::ConfigReader(const QString &filePath) :
    m_confFilePath(filePath)
{
    Init();
}

bool ConfigReader::Init()
{
    auto ReadPointArry = [] (const QJsonArray &jsonArry, PolygonList &polygons) {
        QPolygonF polygon;
        for (int i = 0; i < jsonArry.size(); i++) {
            auto p = jsonArry.at(i).toArray();
            QPointF point(p[0].toDouble(), p[1].toDouble());
            polygon << point;
        }
        polygons.push_back(polygon);
    };
    QFile configFile(m_confFilePath);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("can't open %s", m_confFilePath.toLatin1().data());
        assert(false);
        return false;
    }
    QByteArray configData = configFile.readAll();
    configFile.close();
    QJsonDocument configDoc = QJsonDocument::fromJson(configData);
    auto rootObj = configDoc.object();

    // border
    QPolygonF borderPolygon;
    auto borders = rootObj["shell"].toArray();
    for (auto border : borders) {
        auto borderArry = border.toArray();
        borderPolygon.push_back({borderArry.at(0).toDouble(), borderArry.at(1).toDouble()});
    }
    m_broderPolygons.push_back(borderPolygon);

    // buildings
    auto buildings = rootObj["buildings"].toArray();
    for (auto building : buildings) {
        auto buildingObj = building.toArray();
        ReadPointArry(buildingObj, m_buildingPolygons);
    }

    // climbing_space
    auto climbingSpace = rootObj["climbing_space"].toArray();
    for (auto climbing : climbingSpace) {
        auto climbingArry = climbing.toArray();
        ReadPointArry(climbingArry, m_climbingPolygons);
    }

    // entries
    auto entries = rootObj["entries"].toArray();
    for (auto entry : entries) {
        auto entryArry = entry.toArray();
        ReadPointArry(entryArry, m_entriesPolygons);
    }

    // turning_space
    auto turningSpaces = rootObj["turning_space"].toArray();
    for (auto turningSpace : turningSpaces) {
        auto turningSpaceArry = turningSpace.toArray();
        ReadPointArry(turningSpaceArry, m_turningSpacePolygons);
    }

    // obstacles
    auto obstacles = rootObj["obstacles"].toArray();
    for (auto obstacle : obstacles) {
        auto obstacleArry = obstacle.toArray();
        ReadPointArry(obstacleArry, m_obstaclePolygons);
    }

    // road edges
    auto edges = rootObj["edges"].toArray();
    for (auto edge : edges) {
        auto edgeArry = edge.toArray();
        m_roadEdges.push_back({edgeArry.at(0).toInt(), edgeArry.at(1).toInt()});
    }

    // road nodes
    auto nodes = rootObj["nodes"].toArray();
    for (auto node : nodes) {
        auto nodeArry = node.toArray();
        m_roadNodes.push_back({nodeArry.at(0).toDouble(), nodeArry.at(1).toDouble()});
    }

    // road node labels
    auto labels = rootObj["node_labels"].toArray();
    for (auto label : labels) {
        auto labelI = static_cast<NodeLabel>(label.toInt());
        if (labelI < NodeLabel::Entrance || labelI > NodeLabel::Inflection) {
            assert(false);
        }
        m_roadNodeLabels.push_back(labelI);
    }

    return true;
}

void ConfigReader::ReadPoints(const QJsonObject &jsonObj, QPolygonF &polygon)
{
    QStringList keys = jsonObj.keys();
    if (keys.empty()) {
        return;
    }

    auto ParsePoint = [] (const QString& strPoint) {
        QPointF point;
        QStringList p = strPoint.split(",");
        point.setX(p[0].toDouble());
        point.setY(p[1].toDouble());
        return point;
    };

    polygon << ParsePoint(keys[0]);
    keys.removeAt(0);
    for (int i = 0; i < keys.size(); i++) {
        auto key = keys.at(i);
        auto keyPoint = ParsePoint(key);
        auto headPoint = polygon.front();
        auto lastPoint = polygon.last();

        QJsonObject dataObj = jsonObj[key].toObject();
        auto prev = ParsePoint(dataObj["prev"].toString());
        auto next = ParsePoint(dataObj["next"].toString());

        if (prev == lastPoint) {
            polygon << keyPoint;
            keys.removeAt(i);
            i--;
        } else if (next == headPoint) {
            polygon.insert(0, keyPoint);
            keys.removeAt(i);
            i--;
        }
    }
}
