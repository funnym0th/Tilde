#pragma once
#include <QMainWindow>

class QCloseEvent;
class QSplitter;
class QTextBrowser;
class QStackedWidget;
class QPdfView;
class QPdfDocument;
class QProcess;
class QTimer;
class QAction;
class QMenu;
class QTemporaryDir;
class KMessageWidget;
namespace KTextEditor {
    class Document;
    class View;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void openFile(const QString &filePath);
    void refreshPreview();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    void setupEditor();
    void setupPreview();
    void setupMenuBar();
    void setupConnections();
    void setupContextMenus();
    bool isLatexMode() const;
    void updateWindowTitle();
    QString getTempPath() const;
    void updateRecentFilesActions();
    void addRecentFile(const QString &filePath);

    KTextEditor::Document* codeDocument = nullptr;
    KTextEditor::View* codeView = nullptr;
    QTextBrowser* previewScene = nullptr;
    QSplitter* mainScene = nullptr;
    QStackedWidget* previewStack = nullptr;
    QPdfView* pdfPreview = nullptr;
    QPdfDocument* pdfDocument = nullptr;
    QProcess* latexProcess = nullptr;
    QTimer* latexTimer = nullptr;
    QTemporaryDir* tempDir = nullptr;
    KMessageWidget* errorMessage = nullptr;

    static constexpr int MaxRecentFiles = 10;



    QMenu* recentFilesMenu = nullptr;

    QAction* recentFileActions[MaxRecentFiles];

    QAction* newFileAction = nullptr;

    QAction* openFileAction = nullptr;
    QAction* saveFileAction = nullptr;
    QAction* saveAsFileAction = nullptr;
    QAction* exportPdfAction = nullptr;
    QAction* quitFileAction = nullptr;
    QAction* togglePreviewAction = nullptr;

    QAction* syncScrollingAction = nullptr;
    bool isSyncingScroll = false;
};




