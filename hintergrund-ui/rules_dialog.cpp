#include "rules_dialog.hpp"
#include "ui_rules_dialog.h"
#include "rule_edit_dialog.hpp"
#include "rules/rule_set.hpp"
#include "util/config.hpp"
#include <QMessageBox>

rules_dialog::rules_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rules_dialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    int i = 0;
    for (auto& rule : config::values.rules_manager->rules())
    {
        QString str;
        rule->to_string(str);
        ui->list_rules->addItem(str);
        m_rule_map[i] = rule.get();
        i++;
    }
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

void rules_dialog::on_btn_edit_clicked()
{
    if (!ui->list_rules->selectedItems().empty()) {
        int selected_index = ui->list_rules->currentRow();
        auto* rule = m_rule_map[selected_index];
        auto* dialog = new rule_edit_dialog(this, rule);
        dialog->show();
    } else {
        QMessageBox::warning(this, "Error", "No rule selected");
    }
}


void rules_dialog::add_rule(rule *r)
{
    int index = ui->list_rules->count();
    QString str;
    r->to_string(str);
    ui->list_rules->addItem(str);
    m_rule_map[index] = r;
}

void rules_dialog::refresh_list()
{
    QString str;
    ui->list_rules->clear();
    for (const auto& r : m_rule_map) {
        r->to_string(str);
        ui->list_rules->addItem(str);
    }
}
