#include "MainWindow.hpp"
#include "ScoringService.hpp"

#include <QApplication>
#include <QBoxLayout>
#include <QCheckBox>
#include <QDateTime>
#include <QFileInfo>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTableWidget>
#include <QTabWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_repository(QCoreApplication::applicationDirPath() + "/../data/fll_unearthed.db") {
    setWindowTitle("FLL Unearthed Desktop - C++");
    resize(1220, 820);
    applyTheme();

    QString error;
    if (!m_repository.open(&error)) {
        QMessageBox::critical(this, "Erro no banco", "Nao foi possivel abrir o SQLite:\n" + error);
    }

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(buildDashboardTab(), "Dashboard");
    m_tabs->addTab(buildFormTab(), "Nova rodada");
    setCentralWidget(m_tabs);

    refreshDashboard();
}

void MainWindow::applyTheme() {
    qApp->setStyleSheet(R"CSS(
        QMainWindow, QWidget { background: #020617; color: #e5e7eb; font-family: Arial; font-size: 14px; }
        QTabWidget::pane { border: 1px solid #1e293b; border-radius: 8px; }
        QTabBar::tab { background: #0f172a; color: #cbd5e1; padding: 10px 18px; border-top-left-radius: 8px; border-top-right-radius: 8px; }
        QTabBar::tab:selected { background: #2563eb; color: white; }
        QGroupBox { background: #0f172a; border: 1px solid #1e293b; border-radius: 14px; margin-top: 12px; padding: 14px; font-weight: bold; }
        QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; }
        QLineEdit, QSpinBox { background: #111827; border: 1px solid #334155; border-radius: 8px; padding: 8px; color: #f8fafc; }
        QPushButton { background: #2563eb; border: none; border-radius: 10px; padding: 10px 16px; color: white; font-weight: bold; }
        QPushButton:hover { background: #1d4ed8; }
        QPushButton#danger { background: #dc2626; }
        QPushButton#secondary { background: #334155; }
        QTableWidget { background: #0f172a; gridline-color: #334155; border: 1px solid #1e293b; border-radius: 10px; }
        QHeaderView::section { background: #111827; color: #e5e7eb; padding: 8px; border: none; }
        QCheckBox { spacing: 8px; }
    )CSS");
}

QLabel* MainWindow::kpiCard(const QString& title, const QString& value) {
    auto* label = new QLabel("<div style='color:#94a3b8;font-size:12px;'>" + title + "</div>"
                            "<div style='font-size:28px;font-weight:800;color:#f8fafc;'>" + value + "</div>");
    label->setMinimumHeight(88);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("background:#0f172a;border:1px solid #1e293b;border-radius:14px;padding:12px;");
    return label;
}

QWidget* MainWindow::buildDashboardTab() {
    auto* root = new QWidget;
    auto* layout = new QVBoxLayout(root);
    layout->setSpacing(16);

    auto* hero = new QGroupBox("FLL Unearthed Desktop");
    auto* heroLayout = new QVBoxLayout(hero);
    auto* subtitle = new QLabel("Dashboard desktop em C++ conectado diretamente ao SQLite local. Sem FastAPI, sem Dash e sem navegador.");
    subtitle->setWordWrap(true);
    heroLayout->addWidget(subtitle);
    layout->addWidget(hero);

    auto* toolbar = new QHBoxLayout;
    m_dashboardTeam = new QLineEdit("13199");
    auto* refreshBtn = new QPushButton("Atualizar");
    auto* deleteRoundBtn = new QPushButton("Apagar round selecionado");
    deleteRoundBtn->setObjectName("secondary");
    auto* deleteTeamBtn = new QPushButton("Apagar equipe inteira");
    deleteTeamBtn->setObjectName("danger");

    toolbar->addWidget(new QLabel("Equipe:"));
    toolbar->addWidget(m_dashboardTeam);
    toolbar->addWidget(refreshBtn);
    toolbar->addStretch();
    toolbar->addWidget(deleteRoundBtn);
    toolbar->addWidget(deleteTeamBtn);
    layout->addLayout(toolbar);

    auto* kpiGrid = new QGridLayout;
    m_meanLabel = kpiCard("Pontuacao media");
    m_stdLabel = kpiCard("Desvio padrao");
    m_maxLabel = kpiCard("Melhor round");
    m_roundsLabel = kpiCard("Rounds");
    kpiGrid->addWidget(m_meanLabel, 0, 0);
    kpiGrid->addWidget(m_stdLabel, 0, 1);
    kpiGrid->addWidget(m_maxLabel, 0, 2);
    kpiGrid->addWidget(m_roundsLabel, 0, 3);
    layout->addLayout(kpiGrid);

    auto* charts = new QGridLayout;
    m_lineChart = new LineChartWidget;
    m_barChart = new BarChartWidget;
    charts->addWidget(m_lineChart, 0, 0);
    charts->addWidget(m_barChart, 0, 1);
    layout->addLayout(charts);

    m_roundsTable = new QTableWidget;
    m_roundsTable->setColumnCount(6);
    m_roundsTable->setHorizontalHeaderLabels({"ID", "Evento", "Equipe", "Round", "Data/Hora", "Total"});
    m_roundsTable->horizontalHeader()->setStretchLastSection(true);
    m_roundsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_roundsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_roundsTable, 1);

    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshDashboard);
    connect(deleteRoundBtn, &QPushButton::clicked, this, &MainWindow::deleteSelectedRound);
    connect(deleteTeamBtn, &QPushButton::clicked, this, &MainWindow::deleteAllTeamRounds);

    return root;
}

QWidget* MainWindow::countField(const QString& id, const QString& label, int min, int max) {
    auto* row = new QWidget;
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    auto* spin = new QSpinBox;
    spin->setRange(min, max);
    m_spinInputs[id] = spin;
    layout->addWidget(new QLabel(label));
    layout->addStretch();
    layout->addWidget(spin);
    return row;
}

QWidget* MainWindow::boolField(const QString& id, const QString& label) {
    auto* check = new QCheckBox(label);
    m_checkInputs[id] = check;
    return check;
}

QWidget* MainWindow::buildMissionCard(const QString& title, const QList<QWidget*>& fields) {
    auto* box = new QGroupBox(title);
    auto* layout = new QVBoxLayout(box);
    for (auto* field : fields) layout->addWidget(field);
    return box;
}

QWidget* MainWindow::buildFormTab() {
    auto* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    auto* root = new QWidget;
    auto* layout = new QVBoxLayout(root);
    layout->setSpacing(14);

    auto* dataBox = new QGroupBox("Dados da rodada");
    auto* form = new QGridLayout(dataBox);
    m_eventId = new QLineEdit("EVT-MANUAL");
    m_teamNumber = new QLineEdit("13199");
    m_roundNumber = new QSpinBox;
    m_roundNumber->setRange(1, 999);
    m_matchDatetime = new QLineEdit(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm"));
    m_inspectionOk = new QCheckBox("Inspecao OK (+20)");
    m_precisionTokens = new QSpinBox;
    m_precisionTokens->setRange(0, 6);

    form->addWidget(new QLabel("Evento"), 0, 0);
    form->addWidget(m_eventId, 0, 1);
    form->addWidget(new QLabel("Equipe"), 0, 2);
    form->addWidget(m_teamNumber, 0, 3);
    form->addWidget(new QLabel("Round"), 1, 0);
    form->addWidget(m_roundNumber, 1, 1);
    form->addWidget(new QLabel("Data/Hora"), 1, 2);
    form->addWidget(m_matchDatetime, 1, 3);
    form->addWidget(m_inspectionOk, 2, 0, 1, 2);
    form->addWidget(new QLabel("Tokens de precisao restantes"), 2, 2);
    form->addWidget(m_precisionTokens, 2, 3);
    layout->addWidget(dataBox);

    auto* grid = new QGridLayout;
    int row = 0, col = 0;
    auto addCard = [&](QWidget* card) {
        grid->addWidget(card, row, col);
        col++;
        if (col >= 3) { col = 0; row++; }
    };

    addCard(buildMissionCard("M01 - Limpeza da superficie", {countField("m01_sections_clean", "Partes limpas", 0, 2), boolField("m01_brush_out", "Pincel fora da area")}));
    addCard(buildMissionCard("M02 - Revelar o mapa", {countField("m02_sections_revealed", "Partes reveladas", 0, 3)}));
    addCard(buildMissionCard("M03 - Explorador do poco", {boolField("m03_own_cart_enemy", "Seu carrinho na area adversaria"), boolField("m03_enemy_cart_own", "Carrinho adversario na sua area")}));
    addCard(buildMissionCard("M04 - Recuperacao cuidadosa", {boolField("m04_precious_out", "Artefato valioso fora"), boolField("m04_supports_up", "Estruturas de pe")}));
    addCard(buildMissionCard("M05 - Quem morava aqui?", {boolField("m05_floor_up", "Chao totalmente levantado")}));
    addCard(buildMissionCard("M06 - Forja", {countField("m06_ore_blocks_out", "Blocos fora da forja", 0, 3)}));
    addCard(buildMissionCard("M07 - Levantamento de peso", {boolField("m07_weight_lifted", "Peso levantado"), boolField("m07_weight_off_base", "Peso fora da base")}));
    addCard(buildMissionCard("M08 - Silo", {countField("m08_preserved_out", "Pecas preservadas fora", 0, 3)}));
    addCard(buildMissionCard("M09 - O que esta a venda?", {boolField("m09_roof_up", "Telhado levantado"), boolField("m09_goods_up", "Itens levantados")}));
    addCard(buildMissionCard("M10 - Incline as balancas", {boolField("m10_scale_tilted", "Balanca inclinada"), boolField("m10_plate_removed", "Prato removido")}));
    addCard(buildMissionCard("M11 - Artefatos do pescador", {boolField("m11_artifacts_raised", "Artefatos levantados"), boolField("m11_flag_lowered", "Bandeira abaixada")}));
    addCard(buildMissionCard("M12 - Operacao de salvamento", {boolField("m12_sand_removed", "Areia removida"), boolField("m12_ship_raised", "Navio levantado")}));
    addCard(buildMissionCard("M13 - Reconstrucao da estatua", {boolField("m13_statue_raised", "Estatua levantada")}));
    addCard(buildMissionCard("M14 - Forum de artefatos", {countField("m14_artifacts_in_forum", "Artefatos no forum", 0, 7)}));
    addCard(buildMissionCard("M15 - Marcacao do sitio", {countField("m15_flags_placed", "Bandeiras corretas", 0, 3)}));

    layout->addLayout(grid);

    auto* saveBtn = new QPushButton("Salvar rodada no SQLite local");
    layout->addWidget(saveBtn);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveRound);

    scroll->setWidget(root);
    return scroll;
}

ScoreBreakdown MainWindow::collectBreakdown() const {
    ScoreBreakdown d;
    d.inspectionOk = m_inspectionOk->isChecked();
    d.precisionTokensLeft = m_precisionTokens->value();
    d.m01SectionsClean = m_spinInputs.value("m01_sections_clean")->value();
    d.m01BrushOut = m_checkInputs.value("m01_brush_out")->isChecked();
    d.m02SectionsRevealed = m_spinInputs.value("m02_sections_revealed")->value();
    d.m03OwnCartEnemy = m_checkInputs.value("m03_own_cart_enemy")->isChecked();
    d.m03EnemyCartOwn = m_checkInputs.value("m03_enemy_cart_own")->isChecked();
    d.m04PreciousOut = m_checkInputs.value("m04_precious_out")->isChecked();
    d.m04SupportsUp = m_checkInputs.value("m04_supports_up")->isChecked();
    d.m05FloorUp = m_checkInputs.value("m05_floor_up")->isChecked();
    d.m06OreBlocksOut = m_spinInputs.value("m06_ore_blocks_out")->value();
    d.m07WeightLifted = m_checkInputs.value("m07_weight_lifted")->isChecked();
    d.m07WeightOffBase = m_checkInputs.value("m07_weight_off_base")->isChecked();
    d.m08PreservedOut = m_spinInputs.value("m08_preserved_out")->value();
    d.m09RoofUp = m_checkInputs.value("m09_roof_up")->isChecked();
    d.m09GoodsUp = m_checkInputs.value("m09_goods_up")->isChecked();
    d.m10ScaleTilted = m_checkInputs.value("m10_scale_tilted")->isChecked();
    d.m10PlateRemoved = m_checkInputs.value("m10_plate_removed")->isChecked();
    d.m11ArtifactsRaised = m_checkInputs.value("m11_artifacts_raised")->isChecked();
    d.m11FlagLowered = m_checkInputs.value("m11_flag_lowered")->isChecked();
    d.m12SandRemoved = m_checkInputs.value("m12_sand_removed")->isChecked();
    d.m12ShipRaised = m_checkInputs.value("m12_ship_raised")->isChecked();
    d.m13StatueRaised = m_checkInputs.value("m13_statue_raised")->isChecked();
    d.m14ArtifactsInForum = m_spinInputs.value("m14_artifacts_in_forum")->value();
    d.m15FlagsPlaced = m_spinInputs.value("m15_flags_placed")->value();
    return d;
}

void MainWindow::saveRound() {
    if (m_teamNumber->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validacao", "Informe o numero da equipe.");
        return;
    }

    const auto result = ScoringService::calculate(collectBreakdown());

    RoundRecord record;
    record.eventId = m_eventId->text().trimmed().isEmpty() ? "EVT-MANUAL" : m_eventId->text().trimmed();
    record.teamNumber = m_teamNumber->text().trimmed();
    record.roundNumber = m_roundNumber->value();
    record.matchDatetime = m_matchDatetime->text().trimmed();
    record.missions = result.first;
    record.totalScore = result.second;

    QString error;
    if (!m_repository.saveRound(record, &error)) {
        QMessageBox::critical(this, "Erro ao salvar", error);
        return;
    }

    QMessageBox::information(this, "Rodada salva", QString("Round %1 salvo com %2 pontos.").arg(record.roundNumber).arg(record.totalScore));
    m_dashboardTeam->setText(record.teamNumber);
    refreshDashboard();
    m_tabs->setCurrentIndex(0);
}

void MainWindow::refreshDashboard() {
    QString team = m_dashboardTeam->text().trimmed();
    if (team.isEmpty()) team = "13199";

    QString error;
    const auto rounds = m_repository.listRounds(team, &error);
    const auto metrics = m_repository.metrics(team, &error);

    m_meanLabel->setText(QString("<div style='color:#94a3b8;font-size:12px;'>Pontuacao media</div><div style='font-size:28px;font-weight:800;color:#f8fafc;'>%1</div>").arg(metrics.meanScore, 0, 'f', 2));
    m_stdLabel->setText(QString("<div style='color:#94a3b8;font-size:12px;'>Desvio padrao</div><div style='font-size:28px;font-weight:800;color:#f8fafc;'>%1</div>").arg(metrics.stdScore, 0, 'f', 2));
    m_maxLabel->setText(QString("<div style='color:#94a3b8;font-size:12px;'>Melhor round</div><div style='font-size:28px;font-weight:800;color:#f8fafc;'>%1</div>").arg(metrics.maxScore));
    m_roundsLabel->setText(QString("<div style='color:#94a3b8;font-size:12px;'>Rounds</div><div style='font-size:28px;font-weight:800;color:#f8fafc;'>%1</div>").arg(metrics.rounds));

    m_roundsTable->setRowCount(rounds.size());
    for (int i = 0; i < rounds.size(); ++i) {
        const auto& r = rounds[i];
        m_roundsTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.id)));
        m_roundsTable->setItem(i, 1, new QTableWidgetItem(r.eventId));
        m_roundsTable->setItem(i, 2, new QTableWidgetItem(r.teamNumber));
        m_roundsTable->setItem(i, 3, new QTableWidgetItem(QString::number(r.roundNumber)));
        m_roundsTable->setItem(i, 4, new QTableWidgetItem(r.matchDatetime));
        m_roundsTable->setItem(i, 5, new QTableWidgetItem(QString::number(r.totalScore)));
    }
    m_roundsTable->resizeColumnsToContents();

    m_lineChart->setRounds(rounds);
    m_barChart->setMetrics(metrics);
}

void MainWindow::deleteSelectedRound() {
    const int row = m_roundsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Selecao", "Selecione um round na tabela.");
        return;
    }

    const QString team = m_roundsTable->item(row, 2)->text();
    const int roundNumber = m_roundsTable->item(row, 3)->text().toInt();

    if (QMessageBox::question(this, "Confirmar", QString("Apagar o round %1 da equipe %2?").arg(roundNumber).arg(team)) != QMessageBox::Yes) return;

    QString error;
    if (!m_repository.deleteRound(team, roundNumber, &error)) {
        QMessageBox::critical(this, "Erro", error);
        return;
    }
    refreshDashboard();
}

void MainWindow::deleteAllTeamRounds() {
    const QString team = m_dashboardTeam->text().trimmed();
    if (team.isEmpty()) return;

    if (QMessageBox::question(this, "Confirmar", QString("Apagar todos os rounds da equipe %1?").arg(team)) != QMessageBox::Yes) return;

    QString error;
    if (!m_repository.deleteTeamRounds(team, &error)) {
        QMessageBox::critical(this, "Erro", error);
        return;
    }
    refreshDashboard();
}
