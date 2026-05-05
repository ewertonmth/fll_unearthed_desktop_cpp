#pragma once

#include "RoundModels.hpp"
#include <QSqlDatabase>
#include <QString>

class RoundRepository {
public:
    explicit RoundRepository(QString databasePath);

    bool open(QString* errorMessage = nullptr);
    bool saveRound(const RoundRecord& record, QString* errorMessage = nullptr);
    QList<RoundRecord> listRounds(const QString& teamNumber, QString* errorMessage = nullptr);
    TeamMetrics metrics(const QString& teamNumber, QString* errorMessage = nullptr);
    bool deleteRound(const QString& teamNumber, int roundNumber, QString* errorMessage = nullptr);
    bool deleteTeamRounds(const QString& teamNumber, QString* errorMessage = nullptr);

private:
    QString m_databasePath;
    QSqlDatabase m_db;

    bool ensureSchema(QString* errorMessage = nullptr);
};
