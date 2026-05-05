# FLL Unearthed Desktop C++

Versão desktop em **C++ + Qt 6 + SQLite**
O sistema roda como aplicativo desktop, sem precisar subir backend, API ou navegador.

## O que foi convertido

- Backend camada local de persistência em C++.
- Banco SQLite.
- Regras de pontuação foram migradas para `ScoringService`.
- Dashboard virou interface desktop com Qt Widgets.
- Gráficos foram implementados com widgets customizados em C++.
- Cadastro, listagem, métricas e exclusão de rounds funcionam localmente.

## Estrutura

```txt
fll_unearthed_desktop_cpp/
├── CMakeLists.txt
├── README.md
├── data/
│   └── fll_unearthed.db
└── src/
    ├── main.cpp
    ├── MainWindow.hpp/.cpp
    ├── RoundModels.hpp
    ├── RoundRepository.hpp/.cpp
    ├── ScoringService.hpp/.cpp
    └── ChartWidgets.hpp/.cpp
```

## Dependências

No Arch Linux:

```bash
sudo pacman -S cmake ninja qt6-base
```

No Ubuntu/Debian:

```bash
sudo apt install cmake ninja-build qt6-base-dev qt6-base-dev-tools libqt6sql6-sqlite
```

## Como rodar

```bash
cd fll_unearthed_desktop_cpp
cmake -S . -B build -G Ninja
cmake --build build
./build/FllUnearthedDesktop
```

## Observação sobre banco

O app tenta usar este banco:

```txt
data/fll_unearthed.db
```

Se ele não existir, o próprio aplicativo cria a tabela `rounds` automaticamente.

## Próximos upgrades recomendados

- Exportação PDF do relatório da equipe.
- Importação/exportação CSV.
- Tela de comparação entre equipes.
- Instalador `.AppImage`, `.exe` ou `.deb`.
- Tema visual mais parecido com identidade da equipe.
