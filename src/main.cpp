#include <QApplication>
#include "mainwindow.hpp"
int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MainWindow window;
    if(app.arguments().size() > 1) {
        window.openFile(app.arguments().at(1));
    }
    window.show();
    return app.exec();
}
