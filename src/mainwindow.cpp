#include "mainwindow.hpp"
#include <ktexteditor/editor.h>
#include <ktexteditor/view.h>
#include <QSplitter>
#include <QScreen>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <KActionCollection>
#include <QIcon>
#include <QPrinter>
#include <QTextDocument>
#include <QStackedWidget>
#include <QPdfView>
#include <QPdfDocument>
#include <QProcess>
#include <QTimer>
#include <QFile>
#include <ktexteditor/document.h>
#include <QTextBrowser>
#include <QKeySequence>


void MainWindow::openFile(const QString &filePath) {
    if(!filePath.isEmpty()) {
        codeDocument -> openUrl(QUrl::fromLocalFile(filePath));
        setWindowTitle("Tilde - " + QFileInfo(filePath).fileName());
        refreshPreview();
    }
}

void MainWindow::refreshPreview() {
    QString filePath = codeDocument->url().toLocalFile();
    if (QFileInfo(filePath).suffix().toLower() == "tex") {
        previewStack->setCurrentWidget(pdfPreview);
        latexTimer->start(500);
    } else {
        previewStack->setCurrentWidget(previewScene);
        previewScene->setMarkdown(codeDocument->text());
    }
}


MainWindow::MainWindow(QWidget* parent) : QMainWindow{parent} {
    // Window setup
    setWindowTitle("Tilde");
    setWindowIcon(QIcon::fromTheme("tilde", QIcon("res/tilde.svg")));
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenRes = screen -> availableGeometry();
    resize(screenRes.width() * 0.8, screenRes.height()* 0.8);

    // Core objects
    codeDocument = KTextEditor::Editor::instance() -> createDocument(this);
    codeView = codeDocument -> createView(this);
    for (QAction* act : codeView->actionCollection()->actions()) {
        if (act->shortcut() == QKeySequence::New ||
            act->shortcut() == QKeySequence::Open ||
            act->shortcut() == QKeySequence::Save ||
            act->shortcut() == QKeySequence::SaveAs ||
            act->shortcut() == QKeySequence("Ctrl+E") ||
            act->shortcut() == QKeySequence("Ctrl+P") ||
            act->shortcut() == QKeySequence::Print) {
            act->setShortcut(QKeySequence());
        }
    }
    previewScene = new QTextBrowser(this);
    pdfDocument = new QPdfDocument(this);
    pdfPreview = new QPdfView(this);
    pdfPreview -> setDocument(pdfDocument);
    pdfPreview -> setPageMode(QPdfView::PageMode::MultiPage);

    previewStack = new QStackedWidget(this);
    previewStack -> addWidget(previewScene);
    previewStack -> addWidget(pdfPreview);

    latexTimer = new QTimer(this);
    latexTimer -> setSingleShot(true);
    latexProcess = new QProcess(this);

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
    mainScene -> addWidget(previewStack);
    mainScene -> setSizes({500, 500});

    this->setCentralWidget(mainScene);

    // Menu bar and actions
    QMenu* fileMenuBar = menuBar() -> addMenu("File");
    QMenu* editMenuBar = menuBar() -> addMenu("Edit");
    QMenu* viewMenuBar = menuBar() -> addMenu("View");

    QAction* newFileMenuItem = fileMenuBar -> addAction("New");
    newFileMenuItem->setIcon(QIcon::fromTheme("document-new"));
    newFileMenuItem->setShortcut(QKeySequence::New);
    QAction* openFileMenuItem = fileMenuBar -> addAction("Open...");
    openFileMenuItem -> setIcon(QIcon::fromTheme("document-open"));
    openFileMenuItem->setShortcut(QKeySequence::Open);
    QAction* saveFileMenuItem = fileMenuBar -> addAction("Save");
    saveFileMenuItem -> setIcon(QIcon::fromTheme("document-save"));
    saveFileMenuItem->setShortcut(QKeySequence::Save);
    QAction* saveAsFileMenuItem = fileMenuBar -> addAction("Save as...");
    saveAsFileMenuItem -> setIcon(QIcon::fromTheme("document-save-as"));
    saveAsFileMenuItem->setShortcut(QKeySequence::SaveAs);
    fileMenuBar -> addSeparator();
    QAction* exportPdfFileMenuItem = fileMenuBar -> addAction("Export as PDF...");
    exportPdfFileMenuItem -> setIcon(QIcon::fromTheme("document-export"));
    exportPdfFileMenuItem->setShortcut(QKeySequence("Ctrl+E"));
    fileMenuBar -> addSeparator();
    QAction* quitFileMenuItem = fileMenuBar -> addAction("Quit");
    quitFileMenuItem -> setIcon(QIcon::fromTheme("application-exit"));
    quitFileMenuItem->setShortcut(QKeySequence::Quit);

    QAction* togglePreviewViewMenuItem = viewMenuBar -> addAction("Show Live Preview");
    togglePreviewViewMenuItem->setCheckable(true);
    togglePreviewViewMenuItem->setChecked(true);
    togglePreviewViewMenuItem -> setIcon(QIcon::fromTheme("view-preview"));
    togglePreviewViewMenuItem->setShortcut(QKeySequence("Ctrl+P"));


    connect(newFileMenuItem, &QAction::triggered, this, [this]() {
        codeDocument->closeUrl();
        setWindowTitle("Tilde - Untitled");
        refreshPreview();
    });
    connect(quitFileMenuItem, &QAction::triggered, this, [this](){close();});
    connect(openFileMenuItem, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "Markdown and Latex files (*.md *.tex)");
        openFile(filePath);
    });
    connect(saveAsFileMenuItem, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "Markdown (*.md);;Latex (*.tex)");
        if (!filePath.isEmpty()) {
            codeDocument -> saveAs(QUrl::fromLocalFile(filePath));
            setWindowTitle("Tilde - " + QFileInfo(filePath).fileName());
        }
    });
    connect(saveFileMenuItem, &QAction::triggered, this, [this, saveAsFileMenuItem]() {
        if (!codeDocument -> url().isEmpty()) {
            codeDocument -> save();
        }
        else {
            saveAsFileMenuItem->trigger();
        }
    });

    connect(togglePreviewViewMenuItem, &QAction::triggered, this, [this](bool check) {
        previewStack -> setVisible(check);
    });

    connect(exportPdfFileMenuItem, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getSaveFileName(this, "Save Exported file", QDir::homePath(), "PDF Document (*.pdf)");
        if (!filePath.isEmpty()) {
            QString activeFile = codeDocument->url().toLocalFile();
            if (QFileInfo(activeFile).suffix().toLower() == "tex") {
                QString baseName = QFileInfo(activeFile).baseName();
                QString compiledPdf = QDir::tempPath() + "/" + baseName + ".pdf";

                if (QFile::exists(filePath)) {
                    QFile::remove(filePath);
                }
                QFile::copy(compiledPdf, filePath);
            } else {
                QPrinter printer(QPrinter::HighResolution);
                printer.setOutputFormat(QPrinter::PdfFormat);
                printer.setOutputFileName(filePath);
                previewScene->document()->print(&printer);
            }
        }
    });

    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_undo"));
    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_redo"));
    editMenuBar -> addSeparator();
    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_cut"));
    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_copy"));
    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_paste"));
    editMenuBar -> addSeparator();
    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_select_all"));
    editMenuBar -> addSeparator();
    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_find"));
    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_find_next"));
    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_find_prev"));
    editMenuBar -> addAction(codeView->actionCollection() -> action("edit_replace"));


    viewMenuBar -> addSeparator();
    viewMenuBar -> addAction(codeView->actionCollection() -> action("view_line_numbers"));
    viewMenuBar -> addAction(codeView->actionCollection() -> action("view_dynamic_word_wrap"));
    viewMenuBar -> addSeparator();
    viewMenuBar -> addAction(codeView->actionCollection() -> action("view_inc_font_sizes"));
    viewMenuBar -> addAction(codeView->actionCollection() -> action("view_dec_font_sizes"));
    viewMenuBar -> addAction(codeView->actionCollection() -> action("view_reset_font_sizes"));

    // Connect Document and Preview
    connect(latexTimer, &QTimer::timeout, this, [this](){
        QString filePath = codeDocument->url().toLocalFile();
        if(!filePath.isEmpty()) {
            if (latexProcess -> state() != QProcess::NotRunning) {
                latexProcess -> kill();
                latexProcess -> waitForFinished(100);
            }
            latexProcess -> start("/usr/bin/pdflatex", {
               "-interaction=nonstopmode",
               "-output-directory=" + QDir::tempPath(),
                filePath
            });
        }
    });

    connect(latexProcess, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit && !codeDocument->url().isEmpty()) {
            QString name = QFileInfo(codeDocument->url().toLocalFile()).baseName();
            QString compiledPdf = QDir::tempPath() + "/" + name + ".pdf";
            pdfDocument->load(compiledPdf);
        }
    });

    connect(codeDocument, &KTextEditor::Document::textChanged, this, [this]() {
        refreshPreview();
    });

    // Custom Right-Click Context Menus
    QMenu* editorMenu = new QMenu(codeView);
    codeView->setContextMenu(editorMenu);
    auto populateEditorMenu = [this, exportPdfFileMenuItem, togglePreviewViewMenuItem](QMenu* menu) {
        if (!menu) return;
        menu->clear();
        if (auto act = codeView->actionCollection()->action("edit_undo")) menu->addAction(act);
        if (auto act = codeView->actionCollection()->action("edit_redo")) menu->addAction(act);
        menu->addSeparator();
        if (auto act = codeView->actionCollection()->action("edit_cut")) menu->addAction(act);
        if (auto act = codeView->actionCollection()->action("edit_copy")) menu->addAction(act);
        if (auto act = codeView->actionCollection()->action("edit_paste")) menu->addAction(act);
        menu->addSeparator();
        if (auto act = codeView->actionCollection()->action("edit_select_all")) menu->addAction(act);
        menu->addSeparator();
        if (auto act = codeView->actionCollection()->action("edit_find")) menu->addAction(act);
        if (auto act = codeView->actionCollection()->action("edit_find_next")) menu->addAction(act);
        if (auto act = codeView->actionCollection()->action("edit_find_prev")) menu->addAction(act);
        if (auto act = codeView->actionCollection()->action("edit_replace")) menu->addAction(act);
        menu->addSeparator();
        menu->addAction(exportPdfFileMenuItem);
        menu->addAction(togglePreviewViewMenuItem);
    };
    connect(editorMenu, &QMenu::aboutToShow, this, [editorMenu, populateEditorMenu]() { populateEditorMenu(editorMenu); });
    connect(codeView, &KTextEditor::View::contextMenuAboutToShow, this, [populateEditorMenu](KTextEditor::View* view, QMenu* menu) { populateEditorMenu(menu); });

    previewScene->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(previewScene, &QWidget::customContextMenuRequested, this, [this, exportPdfFileMenuItem, togglePreviewViewMenuItem](const QPoint &pos) {
        QMenu menu(previewScene);
        menu.addAction(exportPdfFileMenuItem);
        menu.addAction(togglePreviewViewMenuItem);
        menu.addSeparator();
        QAction* zoomInAct = menu.addAction(QIcon::fromTheme("zoom-in"), "Zoom In");
        connect(zoomInAct, &QAction::triggered, this, [this]() { previewScene->zoomIn(); });
        QAction* zoomOutAct = menu.addAction(QIcon::fromTheme("zoom-out"), "Zoom Out");
        connect(zoomOutAct, &QAction::triggered, this, [this]() { previewScene->zoomOut(); });
        QAction* zoomResetAct = menu.addAction(QIcon::fromTheme("zoom-original"), "Reset Zoom");
        connect(zoomResetAct, &QAction::triggered, this, [this]() {
            QFont f = previewScene->font();
            f.setPointSize(11);
            previewScene->setFont(f);
        });
        menu.addSeparator();
        QAction* copyAct = menu.addAction(QIcon::fromTheme("edit-copy"), "Copy Selected Text");
        copyAct->setEnabled(previewScene->textCursor().hasSelection());
        connect(copyAct, &QAction::triggered, this, [this]() { previewScene->copy(); });
        menu.exec(previewScene->mapToGlobal(pos));
    });

    pdfPreview->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(pdfPreview, &QWidget::customContextMenuRequested, this, [this, exportPdfFileMenuItem, togglePreviewViewMenuItem](const QPoint &pos) {
        QMenu menu(pdfPreview);
        menu.addAction(exportPdfFileMenuItem);
        menu.addAction(togglePreviewViewMenuItem);
        menu.addSeparator();
        QAction* zoomInAct = menu.addAction(QIcon::fromTheme("zoom-in"), "Zoom In");
        connect(zoomInAct, &QAction::triggered, this, [this]() { pdfPreview->setZoomFactor(pdfPreview->zoomFactor() * 1.25); });
        QAction* zoomOutAct = menu.addAction(QIcon::fromTheme("zoom-out"), "Zoom Out");
        connect(zoomOutAct, &QAction::triggered, this, [this]() { pdfPreview->setZoomFactor(pdfPreview->zoomFactor() * 0.8); });
        QAction* zoomResetAct = menu.addAction(QIcon::fromTheme("zoom-original"), "Reset Zoom (100%)");
        connect(zoomResetAct, &QAction::triggered, this, [this]() { pdfPreview->setZoomFactor(1.0); });
        menu.exec(pdfPreview->mapToGlobal(pos));
    });

};

MainWindow::~MainWindow() {}
