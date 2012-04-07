#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void openFile();
    bool saveFile();
    void about();

private:
    void createActions();
    void createMenus();
    void createStatusBar();
    void setCurrentFile(const QString &fileName);
    void analyseFile(const QString &fileName);
    bool saveAnalysis(const QString &fileName);
    void writeSettings();
    void readSettings();

    QPlainTextEdit *textEdit;
    QString curFile;
    QString curPath;

    QMenu *fileMenu;
    QMenu *aboutMenu;
    QAction *openAction;
    QAction *saveAction;
    QAction *analyseAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
    QAction *exitAction;
};

#endif // MAINWINDOW_H
