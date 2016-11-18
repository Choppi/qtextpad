#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent = 0):
QDialog(parent),
ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->lineEditReplace->setText("");
    ui->checkBoxCase->setChecked(false);
    ui->lineEditSearch->setText("");
}


Dialog::~Dialog()
{
    delete ui;
}

QString Dialog::search() const {
    return _search;
}

QString Dialog::replace() const {
    return _replace;
}

bool Dialog::getCase() const {
    return _case;
}

void Dialog::on_buttonBox_accepted()
{
    _search = ui->lineEditSearch->text();
    _case = ui->checkBoxCase->isChecked();
    _replace = ui->lineEditReplace->text();
}
