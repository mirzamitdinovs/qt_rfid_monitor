cmake_minimum_required(VERSION 3.16)
project(qt_rfid_monitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/app/AppWindow.cpp
  src/app/AppWindow.h
  src/pages/DashboardPage.cpp
  src/pages/DashboardPage.h
  src/pages/SearchPage.cpp
  src/pages/SearchPage.h
  src/data/DataProvider.h
  src/data/InMemoryDataProvider.cpp
  src/data/InMemoryDataProvider.h
  resources.qrc            # ← must exist at project root
)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
