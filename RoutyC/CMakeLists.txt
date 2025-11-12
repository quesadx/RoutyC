cmake_minimum_required(VERSION 3.16)
project(RoutyC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(RoutyC main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        stationtree.cpp
        stationtree.h
        transportgraph.cpp
        transportgraph.h
        pathalgorithm.cpp
        pathalgorithm.h
        filemanager.cpp
        filemanager.h
        graphicsitems.cpp
        graphicsitems.h
        networkmanager.cpp
        networkmanager.h)
target_link_libraries(RoutyC
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

