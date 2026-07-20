cmake_minimum_required(VERSION 3.20)
project(tilde LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(KF6TextEditor REQUIRED)

qt_add_executable(tilde main.cpp MainWindow.cpp)
target_link_libraries(tilde PRIVATE Qt6::Widgets KF6::TextEditor)
