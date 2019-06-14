#include "tags_dialog.hpp"
#include "ui_tags_dialog.h"
#include "util/config.hpp"
#include "tagging/tagger.hpp"
#include <QTableWidgetItem>
#include <QStandardItemModel>

tags_dialog::tags_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tags_dialog)
{
    ui->setupUi(this);

    auto* model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList { "Name", "Weight" });

    if (config::values.tag_manager->loaded()) {
        for (const auto& tag : config::values.tag_manager->tags()) {
            QList<QStandardItem*> tag_item;
            auto* name_item = new QStandardItem(tag->name());
            auto* weight_item = new QStandardItem(QString::number(tag->weight()));
            name_item->setEditable(false);
            weight_item->setEditable(false);

            tag_item.append(name_item);
            tag_item.append(weight_item);
            model->insertRow(0, tag_item);
        }
    }

    ui->tags_table->setModel(model);
    ui->tags_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

tags_dialog::~tags_dialog()
{
    delete ui;
}

void tags_dialog::on_tags_table_activated(const QModelIndex &index)
{
    auto data = ui->tags_table->model()->data(index);

    int i = 1;
}
