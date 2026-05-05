#include "ScoringService.hpp"

QPair<QMap<QString, int>, int> ScoringService::calculate(const ScoreBreakdown& data) {
    const QMap<int, int> precisionTable = {{0, 0}, {1, 10}, {2, 15}, {3, 25}, {4, 35}, {5, 50}, {6, 50}};

    const int inspectionPoints = data.inspectionOk ? 20 : 0;
    const int precisionPoints = precisionTable.value(data.precisionTokensLeft, 0);

    QMap<QString, int> scores;
    scores["m01"] = (data.m01SectionsClean * 10) + (data.m01BrushOut ? 10 : 0);
    scores["m02"] = data.m02SectionsRevealed * 10;
    scores["m03"] = (data.m03OwnCartEnemy ? 30 : 0) + (data.m03EnemyCartOwn ? 10 : 0);
    scores["m04"] = (data.m04PreciousOut ? 30 : 0) + (data.m04SupportsUp ? 10 : 0);
    scores["m05"] = data.m05FloorUp ? 30 : 0;
    scores["m06"] = data.m06OreBlocksOut * 10;
    scores["m07"] = (data.m07WeightLifted ? 20 : 0) + (data.m07WeightOffBase ? 10 : 0);
    scores["m08"] = data.m08PreservedOut * 10;
    scores["m09"] = (data.m09RoofUp ? 20 : 0) + (data.m09GoodsUp ? 10 : 0);
    scores["m10"] = (data.m10ScaleTilted ? 20 : 0) + (data.m10PlateRemoved ? 10 : 0);
    scores["m11"] = (data.m11ArtifactsRaised ? 20 : 0) + (data.m11FlagLowered ? 10 : 0);
    scores["m12"] = (data.m12SandRemoved ? 20 : 0) + (data.m12ShipRaised ? 10 : 0);
    scores["m13"] = data.m13StatueRaised ? 30 : 0;
    scores["m14"] = data.m14ArtifactsInForum * 5;
    scores["m15"] = data.m15FlagsPlaced * 10;

    int total = inspectionPoints + precisionPoints;
    for (const auto& key : scores.keys()) {
        total += scores.value(key);
    }

    return {scores, total};
}
