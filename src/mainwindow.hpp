#pragma once
#include <QMainWindow>
#include <ktexteditor/document.h>
#include <ktexteditor/view.h>
#include <QSplitter>
#include <QTextBrowser>

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
    QSplitter* mainScene;
};
