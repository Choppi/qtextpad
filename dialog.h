#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT;

public:
    explicit Dialog(QWidget *parent);
    ~Dialog();

    QString search() const;
    bool getCase() const;
    QString replace() const;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog *ui;
    QString _search;
    bool _case;
    QString _replace;

};

#endif // DIALOG_H
