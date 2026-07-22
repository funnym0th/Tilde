cmake_minimum_required(VERSION 3.20)
project(tilde VERSION 0.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Pdf PdfWidgets)
find_package(KF6TextEditor REQUIRED)

configure_file(src/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp @ONLY)

qt_add_executable(tilde
    src/main.cpp
    src/mainwindow.cpp
    src/aboutdialog.cpp
    src/aboutdialog.hpp
)

target_include_directories(tilde PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(tilde PRIVATE Qt6::Widgets Qt6::PrintSupport Qt6::Pdf Qt6::PdfWidgets KF6::TextEditor)

include(GNUInstallDirs)
install(TARGETS tilde DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES tilde.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES res/tilde.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

set(CPACK_PACKAGE_NAME "Tilde")
set(CPACK_PACKAGE_VENDOR "funnym0th")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A native C++ Markdown & LaTeX editor built on Qt 6 & KDE KatePart")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CPACK_GENERATOR "TGZ")

include(CPack)

