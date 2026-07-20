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
private:
    KTextEditor::Document* codeDocument;
    KTextEditor::View* codeView;
    QSplitter* mainScene;
    QTextBrowser* previewScene;
};
