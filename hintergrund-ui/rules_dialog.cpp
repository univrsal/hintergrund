#include "rules_dialog.hpp"
#include "ui_rules_dialog.h"
#include "rule_edit_dialog.hpp"

rules_dialog::rules_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rules_dialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
}

rules_dialog::~rules_dialog()
{
    delete ui;
}

void rules_dialog::on_btn_remove_clicked()
{
    if (ui->list_rules->count() < 1)
        return;
    auto* selected = ui->list_rules->selectedItems().first();

    if (selected) {
        delete ui->list_rules->takeItem(ui->list_rules->row(selected));
    }
}

void rules_dialog::on_buttonBox_accepted()
{
    /* TODO: save */
    close();
}

void rules_dialog::on_buttonBox_rejected()
{
    close();
}

void rules_dialog::on_btn_add_new_clicked()
{
    auto* dialog = new rule_edit_dialog(this);
    dialog->select_rule_tab(static_cast<rule_type>(ui->cb_new_type->currentIndex()));
    dialog->show();
}
