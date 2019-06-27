#include "tags_dialog.hpp"
#include "ui_tags_dialog.h"
#include "util/config.hpp"
#include "tagging/tagger.hpp"
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <QMessageBox>

tags_dialog::tags_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tags_dialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->tags_table->setRowCount(config::values.tag_manager->tag_count());
    ui->tags_table->setColumnCount(2);
    int row = 0;
    for (const auto& tag : config::values.tag_manager->tags()) {
        auto* name_item = new QTableWidgetItem(tag->name());
        auto* weight_item = new QTableWidgetItem(QString::number(tag->weight()));
        ui->tags_table->setItem(row, 0, weight_item);
        ui->tags_table->setItem(row++, 1, name_item);
    }
    ui->tags_table->sortByColumn(1, Qt::SortOrder::AscendingOrder);
}

tags_dialog::~tags_dialog()
{
    delete ui;
}

void tags_dialog::on_btn_remove_selected_clicked()
{
    ui->tags_table->removeRow(ui->tags_table->currentRow());
}

void tags_dialog::on_tags_table_cellClicked(int row, int column)
{
    auto weight = ui->tags_table->item(row, 0)->text().toFloat() * 100;
    auto name = ui->tags_table->item(row, 1)->text();
    ui->txt_name->setText(name);
    ui->slider_weight->setValue(weight);
}

void tags_dialog::on_btn_save_clicked()
{
    auto selected_row = ui->tags_table->currentRow();
    if (selected_row > -1) {
        QString weight = QString::number(ui->slider_weight->value() / 100.f);
        auto* weight_item = ui->tags_table->item(selected_row, 0);
        auto* name_item = ui->tags_table->item(selected_row, 1);

        if (weight_item && name_item) {
            auto name = ui->txt_name->text().remove(' ');
            if (name.length() > 0) {
                weight_item->setText(weight);
                name_item->setText(ui->txt_name->text().remove(' '));
            } else {
                QMessageBox::warning(this, "Error", "Invalid tag name");
            }
        }
    } else {
        QMessageBox::warning(this, "Error", "No tag selected");
    }
}

void tags_dialog::on_btn_save_new_clicked()
{
    auto name = ui->txt_name_new->text().remove(' ');
    auto weight = ui->slider_weight_new->value() / 100.f;

    if (name.length() > 0) {
        if (config::values.tag_manager->tag_exists(qPrintable(name))) {
            QMessageBox::warning(this, "Error", "A tag with this id already exists");
        } else {
            int row_count = ui->tags_table->rowCount();
            ui->tags_table->setRowCount(row_count + 1);
            ui->tags_table->setItem(row_count, 0, new QTableWidgetItem(QString::number(weight)));
            ui->tags_table->setItem(row_count, 1, new QTableWidgetItem(name));
        }
    } else {
        QMessageBox::warning(this, "Error", "Invalid tag name");
    }
}

void tags_dialog::on_buttonBox_accepted()
{
    auto* tag_mgr = config::values.tag_manager;
    tag_mgr->clear_tags();

    for (int row = 0; row < ui->tags_table->rowCount(); row++) {
        auto weight = ui->tags_table->item(row, 0)->text().toFloat();
        auto name_qstr = ui->tags_table->item(row, 1)->text();
        auto* name = qPrintable(name_qstr);
        tag_mgr->add_new_tag(name, weight);
    }
    close();
}
