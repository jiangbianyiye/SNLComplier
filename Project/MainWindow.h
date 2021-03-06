#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<functional>

#include <QMainWindow>
#include <QTextEdit>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void on_actionNew_File_triggered();
    void on_actionOpen_File_triggered();
    void on_editorTabWidget_tabCloseRequested(int index);
    void on_actionSave_triggered();
    void on_actionLexical_Analysis_triggered();

    void on_actionSyntax_Analysis_triggered();

    void on_actionClear_message_triggered();

private:
    void setCurrentEditorTabTitle(QString filePath);
    void redirectStdoutToFile(std::ostream & stdOutStream,QString& filePath,std::function<void(void)>& printFun);
    void appendFileToTextEdit(QString& filpath,QTextEdit& textEdit);
};

#endif // MAINWINDOW_H
