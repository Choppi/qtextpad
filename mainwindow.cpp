#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QCloseEvent>
#include <iostream>
#include <QSettings>
#include <QFontDialog>
#include <QInputDialog>
#include <QDebug>

#include <dialog.h>
#include <ui_dialog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    settings("ENSIIE","QTextPad"),
    ui(new Ui::MainWindow)
{
    nvCount = 1;
    ui->setupUi(this);
    ui->tabWidget->removeTab(0);
    on_actionNouveau_triggered();

    documentPaths.clear();
    documentPaths = settings.value("recentDocuments").toStringList();
    updateRecentMenu();
    connect(ui->menuDocuments_r_cents,SIGNAL(triggered(QAction*)),this,SLOT(openRecent(QAction*)));
}

MainWindow::~MainWindow()
{
    settings.setValue("recentDocuments",documentPaths);
    delete ui;
}

void MainWindow::on_actionNouveau_triggered()
{
    int ct = ui->tabWidget->addTab(new QTextEdit(), "Nouveau document "+QString::number(nvCount++));
    ui->tabWidget->setCurrentIndex(ct);
    QTextEdit * editor = currentTextEdit();
    editor->setFont(currentFont);
    connect(editor,SIGNAL(cursorPositionChanged()), this, SLOT(on_cursorPositionChanged()));
}

void MainWindow::on_actionCouper_triggered()
{
    currentTextEdit()->cut();
}

void MainWindow::on_actionCopier_triggered()
{
    currentTextEdit()->copy();
}

void MainWindow::on_actionColler_triggered()
{
    currentTextEdit()->paste();
}

void MainWindow::on_actionS_lectionner_tout_triggered()
{
    currentTextEdit()->selectAll();
}

void MainWindow::on_actionD_faire_triggered()
{
    currentTextEdit()->undo();
}

void MainWindow::on_actionRefaire_triggered()
{
    currentTextEdit()->redo();
}


void MainWindow::on_actionEnregistrer_sous_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregister sous...", "", "Fichiers texte (*.txt)");
    if (fileName.isEmpty())
        return;
    saveFile(fileName);
}


void MainWindow::on_actionEnregistrer_triggered()
{
    if (currentTextEdit()->property("fileName").isValid())
        saveFile(currentTextEdit()->property("fileName").toString());
    else
        on_actionEnregistrer_sous_triggered();
}


void MainWindow::saveFile(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        manageRecentDocument(fileName);
        QTextStream ts(&file);
        ts << currentTextEdit()->toPlainText() ;
        ts.flush();
        file.close();
        currentTextEdit()->setProperty("fileName",fileName);
        currentTextEdit()->document()->setModified(false);
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
                                  QFileInfo(fileName).fileName());
    }
    else
        QMessageBox::critical(this,"Echec de sauvegarde", "Le fichier "+
                              fileName+" n'a pas pu être sauvegarde.");
}


inline QTextEdit* MainWindow::currentTextEdit()
{
    return dynamic_cast<QTextEdit*>(ui->tabWidget->currentWidget());
}

void MainWindow::readFile(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        manageRecentDocument(fileName);
        on_actionNouveau_triggered();
        QTextStream ts(&file);
        currentTextEdit()->setPlainText(ts.readAll());
        currentTextEdit()->setProperty("fileName",fileName);
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
                                  QFileInfo(fileName).fileName());
        file.close();
    }
}

void MainWindow::updateRecentMenu()
{
    ui->menuDocuments_r_cents->clear();

    for (int i = 0; i< documentPaths.length(); i++)
    {
        if (!QFile::exists(documentPaths[i]))
        {
            documentPaths.removeAll(documentPaths[i]);
            i--;
        }
        else
        {
            ui->menuDocuments_r_cents->addAction(documentPaths[i]);
        }
    }
}


void MainWindow::on_actionOuvrir_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Ouvrir un fichier","","Text files (*.txt)");
    if (fileName.isEmpty())
        return;

    readFile(fileName);
}

void MainWindow::on_actionFermer_triggered()
{
    if (currentTextEdit()->document()->isModified())
    {
        if (QMessageBox::question(this,"Modification détectée","Le fichier en cours d'édition a été modifié.\nVoulez-vous le sauvegarder ?",
                              QMessageBox::Yes,QMessageBox::No)== QMessageBox::Yes)
            on_actionEnregistrer_triggered();
    }
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
}

void MainWindow::closeEvent(QCloseEvent * ce)
{
    bool allSaved = true;

    for (int i =0; i < ui->tabWidget->count() ; i++)
    {
        QTextEdit* te = dynamic_cast<QTextEdit*>(ui->tabWidget->widget(i));
        if (te != 0)
            allSaved = allSaved && (!te->document()->isModified());
    }

    if (!allSaved)
    {
        // demande et sauve les fichiers.
        int button = QMessageBox::question(this,"Documents modifiés non sauvegardés","Des fichiers n'ont pas été sauvegardés.\nVoulez vous les enregistrer ?",
                                           QMessageBox::NoAll,QMessageBox::YesAll,QMessageBox::Cancel);
        if (button == QMessageBox::Cancel)
        {
            ce->ignore();
            return ;
        }
        if (button == QMessageBox::YesAll)
        {
            for (int i =0; i < ui->tabWidget->count() ; i++)
            {
                ui->tabWidget->setCurrentIndex(i);
                if (currentTextEdit()->document()->isModified())
                    on_actionEnregistrer_triggered();
            }
        }
    }
    ce->accept();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->setCurrentIndex(index);
    on_actionFermer_triggered();
}



void MainWindow::manageRecentDocument(QString s)
{
    if (documentPaths.contains(s))
    {
        documentPaths.removeAll(s);
    }
    documentPaths.push_front(s);

    while (documentPaths.length() > 6)
        documentPaths.pop_back();

    settings.setValue("recentDocuments",documentPaths);
    updateRecentMenu();
}


void MainWindow::openRecent(QAction *a)
{
    readFile(a->text());
}

void MainWindow::on_cursorPositionChanged() {
 QTextEdit * editor = currentTextEdit();
 QTextCursor cursor = editor->textCursor();

 int line = cursor.blockNumber() + 1;
 int col = cursor.columnNumber() + 1;

 QString message = QString("line: %1 \t col: %2").arg(QString::number(line), QString::number(col));

 ui->statusBar->showMessage(message);
}

void MainWindow::on_actionR_gler_la_police_triggered()
{
    bool ok;
   QFont font = QFontDialog::getFont(&ok,currentFont,this);
    if(ok) {
    currentFont = font;
   settings.setValue("font",currentFont.toString());
   for(int i = 0; i < ui->tabWidget->count(); i++) {
       QTextEdit* te = dynamic_cast<QTextEdit*>(ui->tabWidget->widget(i));
    if(te != NULL) {
     te->setFont(currentFont);
    }
   }

   }
}

void MainWindow::on_actionAller_la_ligne_triggered()
{
    QTextEdit * editor = currentTextEdit();
    QTextCursor cursor = editor->textCursor();
    int currentblock = cursor.blockNumber();
    int maxBlock = editor->document()->blockCount();
    bool ok;
    int i = QInputDialog::getInt(this,tr("Aller à la ligne"),tr("Ligne"), currentblock + 1,1,maxBlock,1,&ok);
    if(ok) {
        int distance = i - currentblock - 1;
        if(distance >=0) {
            cursor.movePosition(QTextCursor::NextBlock,QTextCursor::MoveAnchor,distance);
        }
        else {
            cursor.movePosition(QTextCursor::PreviousBlock,QTextCursor::MoveAnchor, -distance);
        }
        editor->setTextCursor(cursor);
    }

}

/*
void MainWindow::on_action_Rechercher_suivant_triggered() {

    searchNextWord(searchString,useCase,replaceString);
}
*/

void MainWindow::searchNextWord(const QString & search, const bool useCase, const QString & replace){
 if(!search.isEmpty()) {
     QTextEdit * editor = currentTextEdit();
     QTextDocument * doc = editor->document();
     QTextCursor findCursor(doc);
     QTextCursor cursor = editor->textCursor();
     QTextDocument::FindFlags flags = QTextDocument::FindWholeWords;
     if(useCase) {
         flags = QTextDocument::FindCaseSensitively;
     }
     findCursor = doc->find(search,cursor,flags);
     if(!findCursor.isNull()) {
     editor->setTextCursor(findCursor);
     if(!replace.isEmpty()) {
         findCursor.insertText(replace);
     }
     }
 }
}

void MainWindow::on_actionRechercher_triggered()
{
    QString searchString = QString();
    bool useCase = false;
    QString replaceString = QString();
    Dialog * dialog = new Dialog(this);
     int res = dialog->exec();
     if(res == QDialog::Accepted) {
         searchString = dialog->search();
         useCase = dialog->getCase();
         replaceString = dialog->replace();

         searchNextWord(searchString,useCase,replaceString);
     }
}


void MainWindow::on_actionRechercher_suivant_triggered()
{

    Dialog * dialog = new Dialog(this);
    QString searchString = dialog->search();
    bool useCase = dialog->getCase();
    QString replaceString = dialog->replace();

    searchNextWord(searchString,useCase,replaceString);
}
