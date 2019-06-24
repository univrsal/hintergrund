#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"
#include "util/config.hpp"

settings_dialog::settings_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings_dialog)
{
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
}
