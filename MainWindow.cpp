#include "MainWindow.hpp"
#include <ktexteditor/editor.h>
#include <ktexteditor/view.h>
#include <QSplitter>
#include <QScreen>
#include <QScrollBar>

MainWindow::MainWindow(QWidget* parent) : QMainWindow{parent} {
    setWindowTitle("Tilde");

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenRes = screen -> availableGeometry();

    resize(screenRes.width() * 0.8, screenRes.height()* 0.8);

    codeDocument = KTextEditor::Editor::instance() -> createDocument(this);
    codeView = codeDocument -> createView(this);
    previewScene = new QTextBrowser(this);

    codeView -> setConfigValue("scrollbar-minimap", false);
    codeView -> setConfigValue("dynamic-word-wrap", false);
    codeView -> setStatusBarEnabled(false);
    codeView -> setMinimumWidth(this -> width() * 0.2);

    previewScene -> setMaximumWidth(this -> width() * 0.8);

    mainScene = new QSplitter(Qt::Horizontal, this);
    mainScene -> addWidget(codeView);
    mainScene -> addWidget(previewScene);
    mainScene -> setSizes({500, 500});

    this->setCentralWidget(mainScene);
};

MainWindow::~MainWindow() {}


