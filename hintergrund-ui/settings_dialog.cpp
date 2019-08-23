#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"
#include "util/config.hpp"
#include <QFileDialog>
#include <QMessageBox>

settings_dialog::settings_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings_dialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    ui->cb_tag_item_names->setChecked(config::values.tag_image_names);
    ui->spin_max_folder_depth->setValue(config::values.max_folder_depth);
    ui->txt_lib_path->setText(config::values.library_path);
    ui->txt_tag_path->setText(config::values.tag_path);
    ui->txt_rule_path->setText(config::values.rule_path);
    ui->list_file_types->clear();
    for (const auto& type : config::values.file_types)
        ui->list_file_types->addItem(type);
}

settings_dialog::~settings_dialog()
{
    delete ui;
}

void settings_dialog::on_settings_dialog_accepted()
{
    /* The strings are always allocated via strdup
     * and have to be free'd before the new values
     * are added
     */
    free((void*) config::values.library_path);
    free((void*) config::values.tag_path);
    free((void*) config::values.rule_path);
    for (const auto& file_type : config::values.file_types)
        free((void*) file_type);
    config::values.file_types.clear();

    config::values.library_path = strdup(qPrintable(ui->txt_lib_path->text()));
    config::values.tag_path = strdup(qPrintable(ui->txt_tag_path->text()));
    config::values.rule_path = strdup(qPrintable(ui->txt_rule_path->text()));

    for (int i = 0; i < ui->list_file_types->count(); i++) {
        auto* str = qPrintable(ui->list_file_types->item(i)->text());
        config::values.file_types.emplace_back(strdup(str));
    }

    config::values.max_folder_depth = ui->spin_max_folder_depth->value();
    config::values.tag_image_names = ui->cb_tag_item_names->isChecked();
}

void settings_dialog::browse_file(QLineEdit* line_edit, const QString& title)
{
    line_edit->setText(
            QFileDialog::getOpenFileName(this, title, "~/",
                                                  "JSON (*.json);;All files (*.*)"));
}

void settings_dialog::on_btn_lib_path_clicked()
{
    browse_file(ui->txt_lib_path, "Select image library file");
}

void settings_dialog::on_btn_tag_path_clicked()
{
    browse_file(ui->txt_tag_path, "Select tag file");
}

void settings_dialog::on_btn_rule_path_clicked()
{
    browse_file(ui->txt_rule_path, "Select rule file");
}

void settings_dialog::on_btn_add_type_clicked()
{
    if (ui->txt_file_type->text().length() > 0) {
        bool unique = true;
        QString new_type = ui->txt_file_type->text();
        for(int i = 0; i < ui->list_file_types->count(); ++i)
        {
            QListWidgetItem* item = ui->list_file_types->item(i);
            if (new_type == item->text()) {
                unique = false;
                break;
            }
        }

        if (unique) {
            ui->list_file_types->addItem(new_type);
        } else {
            QMessageBox::warning(this, "Error", "This file type already exists");
        }
    }
}

void settings_dialog::on_btn_remove_selected_clicked()
{
    if (ui->list_file_types->count() < 1)
        return;
    auto* first = ui->list_file_types->selectedItems().first();
    if (first) {
        int row = ui->list_file_types->row(first);
        delete ui->list_file_types->takeItem(row);
    }
}

void settings_dialog::on_buttonBox_accepted()
{
    /* Todo: save settings */
    close();
}

void settings_dialog::on_buttonBox_rejected()
{
    close();
}
