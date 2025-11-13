cmake_minimum_required(VERSION 3.16)
project(RoutyC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(RoutyC main.cpp
        MainWindow.cpp
        MainWIndow.h
        MainWindow.ui
        StationTree.cpp
        StationTree.h
        TransportGraph.cpp
        TransportGraph.h
        PathAlgorithm.cpp
        PathAlgorithm.h
        FileManager.cpp
        FIleManager.h
        GraphicsItems.cpp
        GraphicsItems.h
        NetworkManager.cpp
        NetworkManager.h
        CustomGraphicsView.cpp
        CustomGraphicsView.h
        resources.qrc)
target_link_libraries(RoutyC
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

