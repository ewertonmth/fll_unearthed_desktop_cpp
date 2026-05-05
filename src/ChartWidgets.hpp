#pragma once

#include "RoundModels.hpp"
#include <QWidget>

class LineChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit LineChartWidget(QWidget* parent = nullptr);
    void setRounds(QList<RoundRecord> rounds);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QList<RoundRecord> m_rounds;
};

class BarChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit BarChartWidget(QWidget* parent = nullptr);
    void setMetrics(TeamMetrics metrics);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    TeamMetrics m_metrics;
};
