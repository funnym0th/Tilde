#include "mainwindow.hpp"
#include <ktexteditor/editor.h>
#include <ktexteditor/view.h>
#include <QSplitter>
#include <QScreen>
#include <QFontDatabase>
#include <QMenuBar>
#include <QFileDialog>
#include <format>

void MainWindow::openFile(const QString &filePath) {
    if(!filePath.isEmpty()) {
    codeDocument -> openUrl(QUrl::fromLocalFile(filePath));
    setWindowTitle("Tilde - " + filePath);
    }
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow{parent} {
    // Window setup
    setWindowTitle("Tilde");
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenRes = screen -> availableGeometry();
    resize(screenRes.width() * 0.8, screenRes.height()* 0.8);

    // Core objects
    codeDocument = KTextEditor::Editor::instance() -> createDocument(this);
    codeView = codeDocument -> createView(this);
    previewScene = new QTextBrowser(this);

    // Layout
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

    // Menu bar and actions
    QMenu* fileMenuBar = menuBar() -> addMenu("File");
    QMenu* editMenuBar = menuBar() -> addMenu("Edit");
    QMenu* viewMenuBar = menuBar() -> addMenu("View");

    QAction* openFileMenuItem = fileMenuBar -> addAction("Open...");
    QAction* saveFileMenuItem = fileMenuBar -> addAction("Save");
    QAction* saveAsFileMenuItem = fileMenuBar -> addAction("Save as...");
    fileMenuBar -> addSeparator();
    QAction* quitAppMenuItem = fileMenuBar -> addAction("Quit");

    connect(quitAppMenuItem, &QAction::triggered, this, [this](){close();});
    connect(openFileMenuItem, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "Markdown and Latex files (*.md *.tex)");
        if(!filePath.isEmpty()) {
            codeDocument -> openUrl(QUrl::fromLocalFile(filePath));
             setWindowTitle("Tilde - " + filePath);
        }
    });
    connect(saveAsFileMenuItem, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "Markdown (*.md);;Latex (*.tex)");
        if (!filePath.isEmpty()) {
            codeDocument -> saveAs(QUrl::fromLocalFile(filePath));
            setWindowTitle("Tilde - " + filePath);
        }
    });
    connect(saveFileMenuItem, &QAction::triggered, this, [this, saveAsFileMenuItem]() {
        if (!codeDocument->url().isEmpty()) {
            codeDocument -> save();
        }
        else {
            saveAsFileMenuItem->trigger();
        }
    });

    // Connect Document and Preview
    connect(codeDocument, &KTextEditor::Document::textChanged, this, [this]() {
        previewScene->setMarkdown(codeDocument->text());
    });

};

MainWindow::~MainWindow() {}
