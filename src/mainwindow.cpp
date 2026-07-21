#include "mainwindow.hpp"
#include <ktexteditor/editor.h>
#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <ktexteditor/cursor.h>
#include <QSplitter>
#include <QVBoxLayout>
#include <KMessageWidget>
#include <QScreen>

#include <QGuiApplication>
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
#include <QFileInfo>
#include <QDir>
#include <QTemporaryDir>
#include <QScrollBar>
#include <QSettings>
#include <QTextBrowser>
#include <QKeySequence>
#include <QCloseEvent>
#include <QMessageBox>

void MainWindow::closeEvent(QCloseEvent *event) {
    if (codeDocument && !codeDocument->queryClose()) {
        event->ignore();
        return;
    }
    else {
        event->accept();
    }
}

void MainWindow::openFile(const QString &filePath) {
    if (!filePath.isEmpty()) {
        codeDocument->openUrl(QUrl::fromLocalFile(filePath));
        addRecentFile(filePath);
        updateWindowTitle();
        refreshPreview();
    }
}

bool MainWindow::isLatexMode() const {
    QString filePath = codeDocument->url().toLocalFile();
    if (!filePath.isEmpty()) {
        return QFileInfo(filePath).suffix().toLower() == "tex";
    }
    return previewStack->currentWidget() == pdfPreview;
}

void MainWindow::updateWindowTitle() {
    QString fileName = codeDocument->url().isEmpty() 
                       ? "Untitled" 
                       : QFileInfo(codeDocument->url().toLocalFile()).fileName();
    QString modifiedMark = codeDocument->isModified() ? "*" : "";
    setWindowTitle("Tilde - " + fileName + modifiedMark);
}

QString MainWindow::getTempPath() const {
    if (tempDir && tempDir->isValid()) {
        return tempDir->path();
    }
    return QDir::tempPath();
}

void MainWindow::updateRecentFilesActions() {
    QSettings settings("Tilde", "TildeEditor");
    QStringList files = settings.value("recentFiles").toStringList();

    QStringList validFiles;
    for (const QString &file : files) {
        if (QFile::exists(file) && !validFiles.contains(file)) {
            validFiles.append(file);
        }
    }
    if (validFiles != files) {
        settings.setValue("recentFiles", validFiles);
        files = validFiles;
    }

    int numRecentFiles = qMin(files.size(), static_cast<int>(MaxRecentFiles));
    for (int i = 0; i < numRecentFiles; ++i) {

        QString text = QString("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setData(files[i]);
        recentFileActions[i]->setToolTip(files[i]);
        recentFileActions[i]->setVisible(true);
    }
    for (int i = numRecentFiles; i < MaxRecentFiles; ++i) {
        recentFileActions[i]->setVisible(false);
    }
    recentFilesMenu->setEnabled(numRecentFiles > 0);
}

void MainWindow::addRecentFile(const QString &filePath) {
    if (filePath.isEmpty()) return;
    QSettings settings("Tilde", "TildeEditor");
    QStringList files = settings.value("recentFiles").toStringList();
    files.removeAll(filePath);
    files.prepend(filePath);
    while (files.size() > MaxRecentFiles) {
        files.removeLast();
    }
    settings.setValue("recentFiles", files);
    updateRecentFilesActions();
}

void MainWindow::refreshPreview() {
    if (isLatexMode()) {
        previewStack->setCurrentWidget(pdfPreview);
        latexTimer->start(500);
    } else {
        if (errorMessage && errorMessage->isVisible()) {
            errorMessage->animatedHide();
        }
        previewStack->setCurrentWidget(previewScene);
        previewScene->setMarkdown(codeDocument->text());
    }
}


MainWindow::MainWindow(QWidget* parent) : QMainWindow{parent} {
    setWindowTitle("Tilde");
    setWindowIcon(QIcon::fromTheme("tilde", QIcon("res/tilde.svg")));
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenRes = screen->availableGeometry();
    resize(screenRes.width() * 0.8, screenRes.height() * 0.8);

    setupEditor();
    setupPreview();
    setupMenuBar();
    setupConnections();
    setupContextMenus();
}

MainWindow::~MainWindow() {
    if(latexProcess->state() != QProcess::NotRunning) {
        latexProcess->kill();
        latexProcess->waitForFinished(200);
    }
    delete tempDir;
}

void MainWindow::setupEditor() {
    codeDocument = KTextEditor::Editor::instance()->createDocument(this);
    codeView = codeDocument->createView(this);
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
    codeView->setConfigValue("scrollbar-minimap", false);
    codeView->setConfigValue("dynamic-word-wrap", false);
    codeView->setStatusBarEnabled(false);
    codeView->setMinimumWidth(this->width() * 0.2);
    codeView->setStyleSheet("QScrollBar:vertical { width: 0px; }"
                            "QScrollBar:horizontal { height: 0px; }");
}

void MainWindow::setupPreview() {
    previewScene = new QTextBrowser(this);
    previewScene->setMaximumWidth(this->width() * 0.8);

    pdfDocument = new QPdfDocument(this);
    pdfPreview = new QPdfView(this);
    pdfPreview->setDocument(pdfDocument);
    pdfPreview->setPageMode(QPdfView::PageMode::MultiPage);

    previewStack = new QStackedWidget(this);
    previewStack->addWidget(previewScene);
    previewStack->addWidget(pdfPreview);

    latexTimer = new QTimer(this);
    latexTimer->setSingleShot(true);
    latexProcess = new QProcess(this);

    mainScene = new QSplitter(Qt::Horizontal, this);
    mainScene->addWidget(codeView);
    mainScene->addWidget(previewStack);
    mainScene->setSizes({500, 500});

    errorMessage = new KMessageWidget(this);
    errorMessage->setMessageType(KMessageWidget::Error);
    errorMessage->setWordWrap(true);
    errorMessage->setCloseButtonVisible(true);
    errorMessage->hide();

    QWidget* centralContainer = new QWidget(this);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralContainer);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(errorMessage);
    centralLayout->addWidget(mainScene, 1);

    setCentralWidget(centralContainer);

    tempDir = new QTemporaryDir(QDir::tempPath() + "/tilde_XXXXXX");
}


void MainWindow::setupMenuBar() {
    QMenu* fileMenuBar = menuBar()->addMenu("File");
    QMenu* editMenuBar = menuBar()->addMenu("Edit");
    QMenu* viewMenuBar = menuBar()->addMenu("View");

    newFileAction = fileMenuBar->addAction("New");
    newFileAction->setIcon(QIcon::fromTheme("document-new"));
    newFileAction->setShortcut(QKeySequence::New);

    openFileAction = fileMenuBar->addAction("Open...");
    openFileAction->setIcon(QIcon::fromTheme("document-open"));
    openFileAction->setShortcut(QKeySequence::Open);

    recentFilesMenu = fileMenuBar->addMenu("Open Recent");
    recentFilesMenu->setIcon(QIcon::fromTheme("document-open-recent"));
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], &QAction::triggered, this, [this, i]() {
            openFile(recentFileActions[i]->data().toString());
        });
        recentFilesMenu->addAction(recentFileActions[i]);
    }
    updateRecentFilesActions();

    saveFileAction = fileMenuBar->addAction("Save");

    saveFileAction->setIcon(QIcon::fromTheme("document-save"));
    saveFileAction->setShortcut(QKeySequence::Save);

    saveAsFileAction = fileMenuBar->addAction("Save as...");
    saveAsFileAction->setIcon(QIcon::fromTheme("document-save-as"));
    saveAsFileAction->setShortcut(QKeySequence::SaveAs);

    fileMenuBar->addSeparator();

    exportPdfAction = fileMenuBar->addAction("Export as PDF...");
    exportPdfAction->setIcon(QIcon::fromTheme("document-export"));
    exportPdfAction->setShortcut(QKeySequence("Ctrl+E"));

    fileMenuBar->addSeparator();

    quitFileAction = fileMenuBar->addAction("Quit");
    quitFileAction->setIcon(QIcon::fromTheme("application-exit"));
    quitFileAction->setShortcut(QKeySequence::Quit);

    togglePreviewAction = viewMenuBar->addAction("Show Live Preview");
    togglePreviewAction->setCheckable(true);
    togglePreviewAction->setChecked(true);
    togglePreviewAction->setIcon(QIcon::fromTheme("view-preview"));
    togglePreviewAction->setShortcut(QKeySequence("Ctrl+P"));

    syncScrollingAction = viewMenuBar->addAction("Synchronized Scrolling");
    syncScrollingAction->setCheckable(true);
    syncScrollingAction->setChecked(true);
    syncScrollingAction->setIcon(QIcon::fromTheme("view-restore"));


    editMenuBar->addAction(codeView->actionCollection()->action("edit_undo"));
    editMenuBar->addAction(codeView->actionCollection()->action("edit_redo"));
    editMenuBar->addSeparator();
    editMenuBar->addAction(codeView->actionCollection()->action("edit_cut"));
    editMenuBar->addAction(codeView->actionCollection()->action("edit_copy"));
    editMenuBar->addAction(codeView->actionCollection()->action("edit_paste"));
    editMenuBar->addSeparator();
    editMenuBar->addAction(codeView->actionCollection()->action("edit_select_all"));
    editMenuBar->addSeparator();
    editMenuBar->addAction(codeView->actionCollection()->action("edit_find"));
    editMenuBar->addAction(codeView->actionCollection()->action("edit_find_next"));
    editMenuBar->addAction(codeView->actionCollection()->action("edit_find_prev"));
    editMenuBar->addAction(codeView->actionCollection()->action("edit_replace"));

    viewMenuBar->addSeparator();
    viewMenuBar->addAction(codeView->actionCollection()->action("view_line_numbers"));
    viewMenuBar->addAction(codeView->actionCollection()->action("view_dynamic_word_wrap"));
    viewMenuBar->addSeparator();
    viewMenuBar->addAction(codeView->actionCollection()->action("view_inc_font_sizes"));
    viewMenuBar->addAction(codeView->actionCollection()->action("view_dec_font_sizes"));
    viewMenuBar->addAction(codeView->actionCollection()->action("view_reset_font_sizes"));
}

void MainWindow::setupConnections() {
    connect(newFileAction, &QAction::triggered, this, [this]() {
        codeDocument->closeUrl();
        updateWindowTitle();
        refreshPreview();
    });

    connect(quitFileAction, &QAction::triggered, this, [this]() { close(); });

    connect(openFileAction, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "Markdown and LaTeX files (*.md *.tex)");
        openFile(filePath);
    });

    connect(saveAsFileAction, &QAction::triggered, this, [this]() {
        QString filePath = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "Markdown (*.md);;LaTeX (*.tex)");
        if (!filePath.isEmpty()) {
            codeDocument->saveAs(QUrl::fromLocalFile(filePath));
            addRecentFile(filePath);
            updateWindowTitle();
            refreshPreview();
        }
    });

    connect(saveFileAction, &QAction::triggered, this, [this]() {
        if (!codeDocument->url().isEmpty()) {
            codeDocument->save();
        } else {
            saveAsFileAction->trigger();
        }
    });

    connect(togglePreviewAction, &QAction::triggered, this, [this](bool check) {
        previewStack->setVisible(check);
    });

    connect(exportPdfAction, &QAction::triggered, this, [this]() {
        QString activeFile = codeDocument->url().toLocalFile();
        if (activeFile.isEmpty() && isLatexMode()) {
            saveAsFileAction->trigger();
            activeFile = codeDocument->url().toLocalFile();
            if (activeFile.isEmpty()) return;
        }

        QString filePath = QFileDialog::getSaveFileName(this, "Save Exported File", QDir::homePath(), "PDF Document (*.pdf)");
        if (!filePath.isEmpty()) {
            if (isLatexMode()) {
                QString baseName = QFileInfo(activeFile).baseName();
                QString compiledPdf = getTempPath() + "/" + baseName + ".pdf";
                if (QFile::exists(compiledPdf)) {
                    if (QFile::exists(filePath)) {
                        QFile::remove(filePath);
                    }
                    QFile::copy(compiledPdf, filePath);
                }
            } else {
                QPrinter printer(QPrinter::HighResolution);
                printer.setOutputFormat(QPrinter::PdfFormat);
                printer.setOutputFileName(filePath);
                previewScene->document()->print(&printer);
            }
        }
    });

    connect(latexTimer, &QTimer::timeout, this, [this]() {
        QString filePath = codeDocument->url().toLocalFile();
        if (!filePath.isEmpty()) {
            if (latexProcess->state() != QProcess::NotRunning) {
                latexProcess->kill();
                latexProcess->waitForFinished(100);
            }
            latexProcess->start("pdflatex", {
                "-interaction=nonstopmode",
                "-output-directory=" + getTempPath(),
                filePath
            });
        }
    });

    connect(latexProcess, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit && exitCode == 0 && !codeDocument->url().isEmpty()) {
            QString name = QFileInfo(codeDocument->url().toLocalFile()).baseName();
            QString compiledPdf = getTempPath() + "/" + name + ".pdf";
            if (QFile::exists(compiledPdf)) {
                pdfDocument->load(compiledPdf);
            }
            if (errorMessage && errorMessage->isVisible()) {
                errorMessage->animatedHide();
            }
        } else if (errorMessage && isLatexMode()) {
            QString output = QString::fromLocal8Bit(latexProcess->readAllStandardOutput());
            QString errOutput = QString::fromLocal8Bit(latexProcess->readAllStandardError());
            QString combined = output + "\n" + errOutput;

            QStringList lines = combined.split('\n');
            QString firstErrorLine;
            QString contextLine;
            for (int i = 0; i < lines.size(); ++i) {
                QString line = lines[i].trimmed();
                if (line.startsWith('!')) {
                    firstErrorLine = line;
                    for (int j = i + 1; j < qMin(i + 5, lines.size()); ++j) {
                        QString nextLine = lines[j].trimmed();
                        if (nextLine.startsWith("l.")) {
                            contextLine = nextLine;
                            break;
                        }
                    }
                    break;
                }
            }
            QString msg = "Failed to compile LaTeX document: ";
            if (!firstErrorLine.isEmpty()) {
                msg += firstErrorLine;
                if (!contextLine.isEmpty()) {
                    msg += " (" + contextLine + ")";
                }
            } else {
                msg += "Compilation failed with exit code " + QString::number(exitCode) + ". Please check your syntax.";
            }
            errorMessage->setText(msg);
            errorMessage->animatedShow();
        }
    });

    connect(latexProcess, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart && errorMessage && isLatexMode()) {
            errorMessage->setText("Failed to start pdflatex. Please ensure TeX Live / pdflatex is installed and in your PATH.");
            errorMessage->animatedShow();
        }
    });


    connect(codeDocument, &KTextEditor::Document::textChanged, this, [this]() {
        refreshPreview();
    });

    connect(codeDocument, &KTextEditor::Document::modifiedChanged, this, [this]() {
        updateWindowTitle();
    });

    auto syncFromEditor = [this]() {
        if (isSyncingScroll || !syncScrollingAction || !syncScrollingAction->isChecked()) return;

        double ratio = 0.0;
        QScrollBar* codeBar = codeView->verticalScrollBar();
        if (codeBar && codeBar->maximum() > 0) {
            ratio = static_cast<double>(codeBar->value()) / codeBar->maximum();
        } else {
            KTextEditor::Cursor maxPos = codeView->maxScrollPosition();
            if (maxPos.line() > 0 && codeView->cursorPosition().line() >= 0) {
                ratio = static_cast<double>(codeView->cursorPosition().line()) / maxPos.line();
            }
        }
        if (ratio < 0.0) ratio = 0.0;
        if (ratio > 1.0) ratio = 1.0;

        isSyncingScroll = true;
        if (isLatexMode() && pdfPreview && pdfPreview->verticalScrollBar()) {
            QScrollBar* pdfBar = pdfPreview->verticalScrollBar();
            if (pdfBar->maximum() > 0) {
                pdfBar->setValue(qRound(ratio * pdfBar->maximum()));
            }
        } else if (!isLatexMode() && previewScene && previewScene->verticalScrollBar()) {
            QScrollBar* sceneBar = previewScene->verticalScrollBar();
            if (sceneBar->maximum() > 0) {
                sceneBar->setValue(qRound(ratio * sceneBar->maximum()));
            }
        }
        isSyncingScroll = false;
    };

    connect(codeView->verticalScrollBar(), &QScrollBar::valueChanged, this, [syncFromEditor](int) { syncFromEditor(); });
    connect(codeView, &KTextEditor::View::verticalScrollPositionChanged, this, [syncFromEditor](KTextEditor::View*, const KTextEditor::Cursor&) { syncFromEditor(); });
    connect(codeView, &KTextEditor::View::cursorPositionChanged, this, [syncFromEditor](KTextEditor::View*, const KTextEditor::Cursor&) { syncFromEditor(); });

    connect(previewScene->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        if (isSyncingScroll || isLatexMode() || !syncScrollingAction || !syncScrollingAction->isChecked()) return;
        QScrollBar* sceneBar = previewScene->verticalScrollBar();
        QScrollBar* codeBar = codeView->verticalScrollBar();
        if (!sceneBar || sceneBar->maximum() <= 0 || !codeBar || codeBar->maximum() <= 0) return;

        double ratio = static_cast<double>(value) / sceneBar->maximum();
        isSyncingScroll = true;
        codeBar->setValue(qRound(ratio * codeBar->maximum()));
        isSyncingScroll = false;
    });

    connect(pdfPreview->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        if (isSyncingScroll || !isLatexMode() || !syncScrollingAction || !syncScrollingAction->isChecked()) return;
        QScrollBar* pdfBar = pdfPreview->verticalScrollBar();
        QScrollBar* codeBar = codeView->verticalScrollBar();
        if (!pdfBar || pdfBar->maximum() <= 0 || !codeBar || codeBar->maximum() <= 0) return;

        double ratio = static_cast<double>(value) / pdfBar->maximum();
        isSyncingScroll = true;
        codeBar->setValue(qRound(ratio * codeBar->maximum()));
        isSyncingScroll = false;
    });
}

void MainWindow::setupContextMenus() {
    QMenu* editorMenu = new QMenu(codeView);
    codeView->setContextMenu(editorMenu);
    auto populateEditorMenu = [this](QMenu* menu) {
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
        menu->addAction(exportPdfAction);
        menu->addAction(togglePreviewAction);
    };
    connect(editorMenu, &QMenu::aboutToShow, this, [editorMenu, populateEditorMenu]() { populateEditorMenu(editorMenu); });
    connect(codeView, &KTextEditor::View::contextMenuAboutToShow, this, [populateEditorMenu](KTextEditor::View*, QMenu* menu) { populateEditorMenu(menu); });

    previewScene->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(previewScene, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        QMenu menu(previewScene);
        menu.addAction(exportPdfAction);
        menu.addAction(togglePreviewAction);
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
    connect(pdfPreview, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        QMenu menu(pdfPreview);
        menu.addAction(exportPdfAction);
        menu.addAction(togglePreviewAction);
        menu.addSeparator();
        QAction* zoomInAct = menu.addAction(QIcon::fromTheme("zoom-in"), "Zoom In");
        connect(zoomInAct, &QAction::triggered, this, [this]() { pdfPreview->setZoomFactor(pdfPreview->zoomFactor() * 1.25); });
        QAction* zoomOutAct = menu.addAction(QIcon::fromTheme("zoom-out"), "Zoom Out");
        connect(zoomOutAct, &QAction::triggered, this, [this]() { pdfPreview->setZoomFactor(pdfPreview->zoomFactor() * 0.8); });
        QAction* zoomResetAct = menu.addAction(QIcon::fromTheme("zoom-original"), "Reset Zoom (100%)");
        connect(zoomResetAct, &QAction::triggered, this, [this]() { pdfPreview->setZoomFactor(1.0); });
        menu.exec(pdfPreview->mapToGlobal(pos));
    });
}
