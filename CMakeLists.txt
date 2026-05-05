cmake_minimum_required(VERSION 3.20)
project(FllUnearthedDesktop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_executable(FllUnearthedDesktop
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.hpp
    src/ScoringService.cpp
    src/ScoringService.hpp
    src/RoundRepository.cpp
    src/RoundRepository.hpp
    src/RoundModels.hpp
    src/ChartWidgets.cpp
    src/ChartWidgets.hpp
)

target_link_libraries(FllUnearthedDesktop PRIVATE Qt6::Widgets Qt6::Sql)

if (WIN32)
    set_target_properties(FllUnearthedDesktop PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
