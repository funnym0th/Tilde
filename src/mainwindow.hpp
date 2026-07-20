#pragma once
#include <QMainWindow>
#include <ktexteditor/document.h>
#include <QTextBrowser>


class QSplitter;
class QTextBrowser;
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
private:
    KTextEditor::Document* codeDocument = nullptr;
    KTextEditor::View* codeView = nullptr;
    QTextBrowser* previewScene = nullptr;
    QSplitter* mainScene = nullptr;
};
