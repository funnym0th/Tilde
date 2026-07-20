#include <QApplication>
#include <iostream>
#include "mainwindow.hpp"
int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MainWindow window;



    if(app.arguments().size() > 1 ) {
        QString filePath = app.arguments().at(1);
        if(filePath.endsWith(".md") || filePath.endsWith(".tex")) {
            window.openFile(filePath);
            window.show();
            return app.exec();
        }
        else {
            std::cout << "error: invalid file mimetype";
        }
    }
    else {
        window.show();
        return app.exec();
    }
}
