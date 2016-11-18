#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QStringList>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    

    void manageRecentDocument(QString s);

private slots:

    void on_actionNouveau_triggered();

    void on_actionCouper_triggered();

    void on_actionCopier_triggered();

    void on_actionColler_triggered();

    void on_actionS_lectionner_tout_triggered();

    void on_actionD_faire_triggered();

    void on_actionRefaire_triggered();

    void on_actionEnregistrer_sous_triggered();

    void on_actionEnregistrer_triggered();

    void on_actionOuvrir_triggered();

    void on_actionFermer_triggered();

    void on_tabWidget_tabCloseRequested(int index);


protected:
    void closeEvent(QCloseEvent *);

private slots:
    void openRecent(QAction* a);

    void on_actionR_gler_la_police_triggered();

    void on_actionAller_la_ligne_triggered();
    void on_cursorPositionChanged();

    void on_actionRechercher_triggered();

    void on_actionRechercher_suivant_triggered();

private:
    QStringList documentPaths;
    QFont currentFont;

    QTextEdit* currentTextEdit();
    void saveFile(QString fileName);
    void readFile(QString fileName);
    void updateRecentMenu();

    QSettings settings;

    Ui::MainWindow *ui;
    int nvCount;

    void searchNextWord(const QString & search, const bool useCase, const QString & replace);

};

#endif // MAINWINDOW_H
