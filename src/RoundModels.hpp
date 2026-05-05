#pragma once

#include <QString>
#include <QMap>
#include <QStringList>
#include <vector>

struct ScoreBreakdown {
    bool inspectionOk = false;
    int precisionTokensLeft = 0;

    int m01SectionsClean = 0;
    bool m01BrushOut = false;
    int m02SectionsRevealed = 0;
    bool m03OwnCartEnemy = false;
    bool m03EnemyCartOwn = false;
    bool m04PreciousOut = false;
    bool m04SupportsUp = false;
    bool m05FloorUp = false;
    int m06OreBlocksOut = 0;
    bool m07WeightLifted = false;
    bool m07WeightOffBase = false;
    int m08PreservedOut = 0;
    bool m09RoofUp = false;
    bool m09GoodsUp = false;
    bool m10ScaleTilted = false;
    bool m10PlateRemoved = false;
    bool m11ArtifactsRaised = false;
    bool m11FlagLowered = false;
    bool m12SandRemoved = false;
    bool m12ShipRaised = false;
    bool m13StatueRaised = false;
    int m14ArtifactsInForum = 0;
    int m15FlagsPlaced = 0;
};

struct RoundRecord {
    int id = 0;
    QString eventId = "EVT-MANUAL";
    QString teamNumber;
    int roundNumber = 1;
    QString matchDatetime;
    int totalScore = 0;
    QMap<QString, int> missions;
};

struct TeamMetrics {
    QString teamNumber;
    int rounds = 0;
    double meanScore = 0.0;
    double stdScore = 0.0;
    int maxScore = 0;
    QMap<QString, double> missionAverages;
};

static inline QStringList missionKeys() {
    return {"m01", "m02", "m03", "m04", "m05", "m06", "m07", "m08", "m09", "m10", "m11", "m12", "m13", "m14", "m15"};
}

static inline QMap<QString, QString> missionLabels() {
    return {
        {"m01", "M01 - Limpeza da superficie"},
        {"m02", "M02 - Revelar o mapa"},
        {"m03", "M03 - Explorador do poco"},
        {"m04", "M04 - Recuperacao cuidadosa"},
        {"m05", "M05 - Quem morava aqui?"},
        {"m06", "M06 - Forja"},
        {"m07", "M07 - Levantamento de peso"},
        {"m08", "M08 - Silo"},
        {"m09", "M09 - O que esta a venda?"},
        {"m10", "M10 - Incline as balancas"},
        {"m11", "M11 - Artefatos do pescador"},
        {"m12", "M12 - Operacao de salvamento"},
        {"m13", "M13 - Reconstrucao da estatua"},
        {"m14", "M14 - Forum de artefatos"},
        {"m15", "M15 - Marcacao do sitio"}
    };
}
