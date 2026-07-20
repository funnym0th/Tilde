cmake_minimum_required(VERSION 3.20)
project(tilde LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Pdf PdfWidgets)
find_package(KF6TextEditor REQUIRED)

qt_add_executable(tilde
    src/main.cpp
    src/mainwindow.cpp
)
target_link_libraries(tilde PRIVATE Qt6::Widgets Qt6::PrintSupport Qt6::Pdf Qt6::PdfWidgets KF6::TextEditor)

include(GNUInstallDirs)
install(TARGETS tilde DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES tilde.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES res/tilde.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

