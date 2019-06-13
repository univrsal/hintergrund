#include "tags_dialog.hpp"
#include "ui_tags_dialog.h"

tags_dialog::tags_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tags_dialog)
{
    ui->setupUi(this);
}

tags_dialog::~tags_dialog()
{
    delete ui;
}
