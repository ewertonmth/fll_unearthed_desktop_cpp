#pragma once

#include "RoundModels.hpp"
#include <QPair>

class ScoringService {
public:
    static QPair<QMap<QString, int>, int> calculate(const ScoreBreakdown& data);
};
