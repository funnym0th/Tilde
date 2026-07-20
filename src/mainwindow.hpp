#pragma once
#include <QMainWindow>

class QSplitter;
class QTextBrowser;
class QStackedWidget;
class QPdfView;
class QPdfDocument;
class QProcess;
class QTimer;
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
private:
    KTextEditor::Document* codeDocument = nullptr;
    KTextEditor::View* codeView = nullptr;
    QTextBrowser* previewScene = nullptr;
    QSplitter* mainScene = nullptr;
    QStackedWidget* previewStack = nullptr;
    QPdfView* pdfPreview = nullptr;
    QPdfDocument* pdfDocument = nullptr;
    QProcess* latexProcess = nullptr;
    QTimer* latexTimer = nullptr;
};
