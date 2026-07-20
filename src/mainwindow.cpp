#include "mainwindow.hpp"
#include <ktexteditor/editor.h>
#include <ktexteditor/view.h>
#include <QSplitter>
#include <QScreen>
#include <QFontDatabase>
#include <QMenuBar>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent) : QMainWindow{parent} {
    setWindowTitle("Tilde");

    QMenu* fileMenuBar = menuBar() -> addMenu("File");
    QMenu* editMenuBar = menuBar() -> addMenu("Edit");
    QMenu* viewMenuBar = menuBar() -> addMenu("View");

    QAction* openFileMenuItem = fileMenuBar -> addAction("Open...");
    QAction* saveFileMenuItem = fileMenuBar -> addAction("Save");
    fileMenuBar -> addSeparator();
    QAction* quitAppMenuItem = fileMenuBar -> addAction("Quit");

    // Quit action
    connect(quitAppMenuItem, &QAction::triggered, this, [this](){close();});
    // Open file action
    connect(openFileMenuItem, &QAction::triggered, this, [this]() {

        QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "Markdown and Latex files (*.md *.tex)");
        if(!filePath.isEmpty()) {
            codeDocument -> openUrl(QUrl::fromLocalFile(filePath));
        }
    });


    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenRes = screen -> availableGeometry();

    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    resize(screenRes.width() * 0.8, screenRes.height()* 0.8);

    codeDocument = KTextEditor::Editor::instance() -> createDocument(this);
    codeDocument->setHighlightingMode("Markdown");
    codeView = codeDocument -> createView(this);
    previewScene = new QTextBrowser(this);

    connect(codeDocument, &KTextEditor::Document::textChanged, this, [this]() {
        previewScene->setMarkdown(codeDocument->text());
    });

    codeView -> setConfigValue("scrollbar-minimap", false);
    codeView -> setConfigValue("dynamic-word-wrap", false);
    codeView -> setStatusBarEnabled(false);
    codeView -> setMinimumWidth(this -> width() * 0.2);
    codeView->setStyleSheet("QScrollBar:vertical { width: 0px; }"
                            "QScrollBar:horizontal { height: 0px; }");

    previewScene -> setMaximumWidth(this -> width() * 0.8);

    mainScene = new QSplitter(Qt::Horizontal, this);
    mainScene -> addWidget(codeView);
    mainScene -> addWidget(previewScene);
    mainScene -> setSizes({500, 500});

    this->setCentralWidget(mainScene);
};

MainWindow::~MainWindow() {}
