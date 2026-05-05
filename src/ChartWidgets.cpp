#include "ChartWidgets.hpp"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <algorithm>

LineChartWidget::LineChartWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(260);
}

void LineChartWidget::setRounds(QList<RoundRecord> rounds) {
    m_rounds = std::move(rounds);
    update();
}

void LineChartWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#0f172a"));
    p.setPen(QColor("#e5e7eb"));
    p.drawText(18, 28, "Pontuacao total por round");

    QRect area = rect().adjusted(48, 52, -24, -42);
    p.setPen(QColor("#334155"));
    p.drawRect(area);

    if (m_rounds.isEmpty()) {
        p.setPen(QColor("#94a3b8"));
        p.drawText(area, Qt::AlignCenter, "Sem dados ainda");
        return;
    }

    int maxScore = 1;
    for (const auto& r : m_rounds) maxScore = std::max(maxScore, r.totalScore);

    QVector<QPointF> points;
    for (int i = 0; i < m_rounds.size(); ++i) {
        double x = area.left() + (m_rounds.size() == 1 ? area.width() / 2.0 : (double)i / (m_rounds.size() - 1) * area.width());
        double y = area.bottom() - (double)m_rounds[i].totalScore / maxScore * area.height();
        points.append(QPointF(x, y));
    }

    p.setPen(QPen(QColor("#38bdf8"), 3));
    for (int i = 1; i < points.size(); ++i) p.drawLine(points[i - 1], points[i]);

    p.setBrush(QColor("#22c55e"));
    p.setPen(Qt::NoPen);
    for (const auto& pt : points) p.drawEllipse(pt, 5, 5);

    p.setPen(QColor("#cbd5e1"));
    for (int i = 0; i < m_rounds.size(); ++i) {
        p.drawText(QPointF(points[i].x() - 10, area.bottom() + 22), QString::number(m_rounds[i].roundNumber));
    }
}

BarChartWidget::BarChartWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(300);
}

void BarChartWidget::setMetrics(TeamMetrics metrics) {
    m_metrics = std::move(metrics);
    update();
}

void BarChartWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#0f172a"));
    p.setPen(QColor("#e5e7eb"));
    p.drawText(18, 28, "Media por missao");

    QRect area = rect().adjusted(48, 52, -24, -54);
    p.setPen(QColor("#334155"));
    p.drawRect(area);

    if (m_metrics.rounds == 0) {
        p.setPen(QColor("#94a3b8"));
        p.drawText(area, Qt::AlignCenter, "Sem dados ainda");
        return;
    }

    double maxValue = 1.0;
    for (const auto& key : missionKeys()) maxValue = std::max(maxValue, m_metrics.missionAverages.value(key, 0.0));

    const auto keys = missionKeys();
    const int gap = 6;
    const double barW = (area.width() - gap * (keys.size() - 1)) / (double)keys.size();

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#22c55e"));
    for (int i = 0; i < keys.size(); ++i) {
        double value = m_metrics.missionAverages.value(keys[i], 0.0);
        double h = value / maxValue * area.height();
        QRectF bar(area.left() + i * (barW + gap), area.bottom() - h, barW, h);
        p.drawRoundedRect(bar, 3, 3);
    }

    p.setPen(QColor("#cbd5e1"));
    for (int i = 0; i < keys.size(); ++i) {
        p.drawText(QPointF(area.left() + i * (barW + gap), area.bottom() + 22), keys[i].toUpper());
    }
}
