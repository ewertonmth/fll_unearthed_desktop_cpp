#pragma once

#include "RoundRepository.hpp"
#include "ChartWidgets.hpp"
#include <QMainWindow>
#include <QMap>

class QLineEdit;
class QSpinBox;
class QCheckBox;
class QLabel;
class QTableWidget;
class QTabWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void saveRound();
    void refreshDashboard();
    void deleteSelectedRound();
    void deleteAllTeamRounds();

private:
    RoundRepository m_repository;

    QTabWidget* m_tabs = nullptr;
    QLineEdit* m_dashboardTeam = nullptr;
    QLabel* m_meanLabel = nullptr;
    QLabel* m_stdLabel = nullptr;
    QLabel* m_maxLabel = nullptr;
    QLabel* m_roundsLabel = nullptr;
    QTableWidget* m_roundsTable = nullptr;
    LineChartWidget* m_lineChart = nullptr;
    BarChartWidget* m_barChart = nullptr;

    QLineEdit* m_eventId = nullptr;
    QLineEdit* m_teamNumber = nullptr;
    QLineEdit* m_matchDatetime = nullptr;
    QSpinBox* m_roundNumber = nullptr;
    QCheckBox* m_inspectionOk = nullptr;
    QSpinBox* m_precisionTokens = nullptr;

    QMap<QString, QSpinBox*> m_spinInputs;
    QMap<QString, QCheckBox*> m_checkInputs;

    QWidget* buildDashboardTab();
    QWidget* buildFormTab();
    QWidget* buildMissionCard(const QString& title, const QList<QWidget*>& fields);
    QWidget* countField(const QString& id, const QString& label, int min, int max);
    QWidget* boolField(const QString& id, const QString& label);
    QLabel* kpiCard(const QString& title, const QString& value = "-");
    ScoreBreakdown collectBreakdown() const;
    void applyTheme();
};
