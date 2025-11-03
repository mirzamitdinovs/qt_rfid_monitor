cmake_minimum_required(VERSION 3.16)
project(qt_rfid_monitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ────────────────────────────────
# Qt modules
# ────────────────────────────────
# Gui is required for fonts (QFontDatabase), Widgets for UI, Sql for QSql*
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Sql)

# ────────────────────────────────
# Target & sources
# ────────────────────────────────
add_executable(${PROJECT_NAME}
  src/main.cpp

  # App
  src/app/AppWindow.cpp
  src/app/AppWindow.h

  # Pages
  src/pages/dashboard/DashboardPage.cpp
  src/pages/dashboard/DashboardPage.h
  src/pages/search/SearchPage.cpp
  src/pages/search/SearchPage.h

  # Data providers
  src/data/DataProvider.h
  src/data/InMemoryDataProvider.cpp
  src/data/InMemoryDataProvider.h
  src/data/MySqlDataProvider.cpp
  src/data/MySqlDataProvider.h

  # Qt resources (contains styles/app.qss and fonts/NotoSansKR-Regular.ttf)
  resources.qrc
)

# ────────────────────────────────
# Include paths & link Qt
# ────────────────────────────────
target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Qt6::Widgets
    Qt6::Gui      # ← needed for fonts
    Qt6::Sql
)
