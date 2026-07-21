#include <QApplication>
#include <QFileInfo>
#include <QUrl>
#include <iostream>
#include "mainwindow.hpp"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MainWindow window;

    if (app.arguments().size() > 1) {
        QString arg = app.arguments().at(1);
        QString filePath = arg.startsWith("file://") ? QUrl(arg).toLocalFile() : QUrl::fromUserInput(arg).toLocalFile();
        if (filePath.isEmpty()) {
            filePath = arg;
        }
        filePath = QFileInfo(filePath).absoluteFilePath();
        QString suffix = QFileInfo(filePath).suffix().toLower();
        if (suffix == "md" || suffix == "tex") {
            window.openFile(filePath);
        } else {
            std::cerr << "error: unsupported file extension '" << filePath.toStdString() << "'\n";
            return 1;
        }
    }

    window.show();
    return app.exec();
}

