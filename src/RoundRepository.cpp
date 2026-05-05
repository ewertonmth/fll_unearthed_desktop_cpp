#include "RoundRepository.hpp"

#include <QDir>
#include <QFileInfo>
#include <utility>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtMath>

RoundRepository::RoundRepository(QString databasePath) : m_databasePath(std::move(databasePath)) {}

bool RoundRepository::open(QString* errorMessage) {
    QFileInfo info(m_databasePath);
    QDir().mkpath(info.absolutePath());

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_databasePath);

    if (!m_db.open()) {
        if (errorMessage) *errorMessage = m_db.lastError().text();
        return false;
    }
    return ensureSchema(errorMessage);
}

bool RoundRepository::ensureSchema(QString* errorMessage) {
    QSqlQuery query(m_db);
    const QString sql = R"SQL(
        CREATE TABLE IF NOT EXISTS rounds (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            event_id TEXT,
            team_number TEXT NOT NULL,
            round_number INTEGER NOT NULL,
            match_datetime TEXT,
            total_score INTEGER NOT NULL,
            m01 INTEGER DEFAULT 0,
            m02 INTEGER DEFAULT 0,
            m03 INTEGER DEFAULT 0,
            m04 INTEGER DEFAULT 0,
            m05 INTEGER DEFAULT 0,
            m06 INTEGER DEFAULT 0,
            m07 INTEGER DEFAULT 0,
            m08 INTEGER DEFAULT 0,
            m09 INTEGER DEFAULT 0,
            m10 INTEGER DEFAULT 0,
            m11 INTEGER DEFAULT 0,
            m12 INTEGER DEFAULT 0,
            m13 INTEGER DEFAULT 0,
            m14 INTEGER DEFAULT 0,
            m15 INTEGER DEFAULT 0
        )
    )SQL";

    if (!query.exec(sql)) {
        if (errorMessage) *errorMessage = query.lastError().text();
        return false;
    }
    return true;
}

bool RoundRepository::saveRound(const RoundRecord& r, QString* errorMessage) {
    QSqlQuery query(m_db);
    query.prepare(R"SQL(
        INSERT INTO rounds (
            event_id, team_number, round_number, match_datetime, total_score,
            m01, m02, m03, m04, m05, m06, m07, m08, m09, m10, m11, m12, m13, m14, m15
        ) VALUES (
            :event_id, :team_number, :round_number, :match_datetime, :total_score,
            :m01, :m02, :m03, :m04, :m05, :m06, :m07, :m08, :m09, :m10, :m11, :m12, :m13, :m14, :m15
        )
    )SQL");

    query.bindValue(":event_id", r.eventId);
    query.bindValue(":team_number", r.teamNumber.trimmed());
    query.bindValue(":round_number", r.roundNumber);
    query.bindValue(":match_datetime", r.matchDatetime);
    query.bindValue(":total_score", r.totalScore);
    for (const auto& key : missionKeys()) {
        query.bindValue(":" + key, r.missions.value(key, 0));
    }

    if (!query.exec()) {
        if (errorMessage) *errorMessage = query.lastError().text();
        return false;
    }
    return true;
}

QList<RoundRecord> RoundRepository::listRounds(const QString& teamNumber, QString* errorMessage) {
    QList<RoundRecord> output;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM rounds WHERE team_number = :team_number ORDER BY round_number ASC, id ASC");
    query.bindValue(":team_number", teamNumber.trimmed());

    if (!query.exec()) {
        if (errorMessage) *errorMessage = query.lastError().text();
        return output;
    }

    while (query.next()) {
        RoundRecord r;
        r.id = query.value("id").toInt();
        r.eventId = query.value("event_id").toString();
        r.teamNumber = query.value("team_number").toString();
        r.roundNumber = query.value("round_number").toInt();
        r.matchDatetime = query.value("match_datetime").toString();
        r.totalScore = query.value("total_score").toInt();
        for (const auto& key : missionKeys()) {
            r.missions[key] = query.value(key).toInt();
        }
        output.append(r);
    }

    return output;
}

TeamMetrics RoundRepository::metrics(const QString& teamNumber, QString* errorMessage) {
    const auto rounds = listRounds(teamNumber, errorMessage);
    TeamMetrics m;
    m.teamNumber = teamNumber.trimmed();
    for (const auto& key : missionKeys()) {
        m.missionAverages[key] = 0.0;
    }

    if (rounds.isEmpty()) return m;

    m.rounds = rounds.size();
    double total = 0.0;
    for (const auto& r : rounds) {
        total += r.totalScore;
        m.maxScore = qMax(m.maxScore, r.totalScore);
        for (const auto& key : missionKeys()) {
            m.missionAverages[key] += r.missions.value(key, 0);
        }
    }
    m.meanScore = total / rounds.size();

    double variance = 0.0;
    for (const auto& r : rounds) {
        variance += qPow(r.totalScore - m.meanScore, 2);
    }
    m.stdScore = rounds.size() > 1 ? qSqrt(variance / rounds.size()) : 0.0;

    for (const auto& key : missionKeys()) {
        m.missionAverages[key] /= rounds.size();
    }
    return m;
}

bool RoundRepository::deleteRound(const QString& teamNumber, int roundNumber, QString* errorMessage) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM rounds WHERE team_number = :team_number AND round_number = :round_number");
    query.bindValue(":team_number", teamNumber.trimmed());
    query.bindValue(":round_number", roundNumber);

    if (!query.exec()) {
        if (errorMessage) *errorMessage = query.lastError().text();
        return false;
    }
    return true;
}

bool RoundRepository::deleteTeamRounds(const QString& teamNumber, QString* errorMessage) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM rounds WHERE team_number = :team_number");
    query.bindValue(":team_number", teamNumber.trimmed());

    if (!query.exec()) {
        if (errorMessage) *errorMessage = query.lastError().text();
        return false;
    }
    return true;
}
