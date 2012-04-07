#include<QtGui>
#include<QTime>
#include<QDate>
#include<QDateTime>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include "mainwindow.h"
#include "intelhexclass.h"

MainWindow::MainWindow()
{
    textEdit = new QPlainTextEdit;
    textEdit->setReadOnly(true);
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createStatusBar();

    setCurrentFile("");

    readSettings();

    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                      "Open Intel HEX File for analysis", curPath,
                      "Intel HEX Files (*.hex)");

    if (!fileName.isEmpty())
        analyseFile(fileName);
}

bool MainWindow::saveFile()
{
    //QString fileName = QFileDialog::getSaveFileName(this);
    QString fileName = QFileDialog::getSaveFileName(this, "Save qtIntegrity analysis", curPath, "Text File (*.txt)");
    if(fileName.isEmpty())
        return false;

    return saveAnalysis(fileName);
}

void MainWindow::about()
{
    QMessageBox msgBox(this);

    msgBox.setIconPixmap(QPixmap(":/images/intelhexclass.png"));

    msgBox.setText(tr("The <b>qtIntegrity</b> application analyses the "
                   "content of an Intel HEX file for correctness. Any "
                   "errors with the file (such as checksum errors, "
                   "unknown records, incorrect construction) will "
                   "be listed. The original file will not be modified. "
                   "<p>Issues marked <b>ERROR</b> mean that the file is corrupted "
                   "and its contents is no longer usable. <p>Issues marked "
                   "<b>WARNING</b> highlight issues with the construction "
                   "of the file that do not affect the binary content, "
                   "which can still be fully recovered."));
    msgBox.setParent(this);

    msgBox.exec();
}

void MainWindow::createActions()
{
    openAction = new QAction(tr("&Open HEX"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Analyse Intel HEX file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    saveAction = new QAction(tr("&Save Analysis"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save results of analysis"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
    aboutMenu->addAction(aboutQtAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Use File->Open to open a HEX file to analyse"));
}

void MainWindow::analyseFile(const QString &fileName)
{
    intelhex ihFile;
    std::ifstream intelHexInput;
    QTime t;
    int elapsedTime;

    intelHexInput.open(fileName.toAscii(), ifstream::in);

    if(!intelHexInput.good())
    {
        QMessageBox::warning(this, tr("qtIntegrity"),
                             tr("Cannot read file %1.")
                             .arg(fileName));
    }



#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    //textEdit->setPlainText(in.readAll());

    t.start();

    intelHexInput >> ihFile;

    elapsedTime = t.elapsed();

    if (ihFile.getNoWarnings() < 1 && ihFile.getNoErrors() < 1)
    {
        QString outputMessage;

        outputMessage += tr("No issues found in file %1.") .arg(fileName);
        outputMessage += tr("\nDecode took %1ms.\n") .arg(elapsedTime);

        QDateTime dateTime = QDateTime::currentDateTime();
        QString dateTimeString = dateTime.toString();
        outputMessage += tr("Date and time: ") + dateTimeString + "\n";

        textEdit->setPlainText(outputMessage);
        statusBar()->showMessage(tr("No issues in file"));
    }
    else
    {
        QString outputMessage;

        statusBar()->showMessage(tr("Issues found in file"));
        outputMessage = tr("File %1 contains the following issues:\n") .arg(fileName);

        if (ihFile.getNoWarnings() > 0)
        {
            outputMessage += "\n";

            while(ihFile.getNoWarnings() > 0)
            {
                QString qmessage;
                string message;

                ihFile.popNextWarning(message);

                qmessage = QString::fromStdString(message);

                outputMessage += tr("WARNING: ");
                outputMessage += qmessage;
                outputMessage += "\n";
            }
        }
        if (ihFile.getNoErrors() > 0)
        {
            outputMessage += "\n";

            while (ihFile.getNoErrors() > 0)
            {
                QString qmessage;
                string message;

                ihFile.popNextError(message);

                qmessage = QString::fromStdString(message);

                outputMessage += tr("ERROR: ") + qmessage + "\n";
            }
        }

        outputMessage += tr("\nDecode took %1ms.\n") .arg(elapsedTime);

        QDateTime dateTime = QDateTime::currentDateTime();
        QString dateTimeString = dateTime.toString();
        outputMessage += tr("Date and time: ") + dateTimeString + "\n";

        textEdit->setPlainText(outputMessage);
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
}

bool MainWindow::saveAnalysis(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("qtIntegrity"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("Analysis saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    curPath = QFileInfo(fileName).path();
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if(curFile.isEmpty())
        shownName = "No File";
    setWindowFilePath(shownName);
}

void MainWindow::readSettings()
{
    QSettings settings("/* CODINGHEAD */", "qtIntegrity");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(700,400)).toSize();
    curPath = settings.value("path", QString("")).toString();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("/* CODINGHEAD */", "qtIntegrity");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("path", curPath);
}
