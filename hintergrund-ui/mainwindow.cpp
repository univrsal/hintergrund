/* mainwindow.cpp created on 2019.5.28
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * github.com/univrsal/
 *
 */
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "ui_helper.hpp"
#include "tags_dialog.hpp"
#include "util/config.hpp"
#include "util/util.hpp"
#include "images/image.hpp"
#include "tagging/tag.hpp"
#include "tagging/tagger.hpp"
#include "settings_dialog.hpp"
#include "rules_dialog.hpp"
#include <QMessageBox>
#include <QFileDialog>
#include <QImageReader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    config::init_config();
    ui->lbl_image->setBackgroundRole(QPalette::Base);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
}

MainWindow::~MainWindow()
{
    config::close_config();
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    if (ui->cb_fit->isChecked() && m_selected_image) {
        load_image(m_selected_image_path);
    }
}

bool MainWindow::load_image(const QString &path)
{
    QImageReader reader(path);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::warning(this, "Error", "Couldn't load image file");
        return false;
    }

    ui->scrollArea->setWidgetResizable(!ui->cb_fit->isChecked());
    if (ui->cb_fit->isChecked()) {
        int w = ui->scrollArea->width() - 10;
        int h = ui->scrollArea->height() - 10;
        ui->lbl_image->resize(w, h);
        ui->scrollAreaWidgetContents->resize(w, h);
        ui->lbl_image->setPixmap(QPixmap::fromImage(newImage.scaled(
                                                        w, h, Qt::KeepAspectRatio)));
    } else {
        ui->lbl_image->setPixmap(QPixmap::fromImage(newImage));
        ui->lbl_image->adjustSize();
    }

    return true;
}

void MainWindow::on_actionOpen_triggered()
{
    /* Get config path via file dialog */
    QString cfg_path =
            QFileDialog::getOpenFileName(this, tr("Select config file"), "~/",
                                                  "JSON (*.json);;All files (*.*)");

    if (strlen(config::values.config_path) > 0)
        free((void*) config::values.config_path);

    config::values.config_path = strdup(qPrintable(cfg_path));
    int ret = 0;
    config::read_config(&ret);
    switch (ret) {
    case config::READ_TAGS_FAILED:
        QMessageBox::critical(this, "Error", "Reading tags file failed");
        break;
    case config::READ_RULES_FAILED:
        QMessageBox::critical(this, "Error", "Reading rules failed");
        break;
    case config::READ_LIBRARY_FAILED:
        QMessageBox::critical(this, "Error", "Reading image library failed");
        break;
    default:
        QMessageBox::critical(this, "Error", "An unknown error occured");
        break;
    case config::SUCCESS:
        ui_helper::populate_file_tree(ui->file_tree);
        ui->actionSettings->setEnabled(true);
        ui->actionRules->setEnabled(true);
        ui->actionView_Tags->setEnabled(true);
        ui->actionShow_preview->setEnabled(true);
        ui->actionOpen->setEnabled(false);
        break;
    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About hintergrund-ui",
                       "<h2>hintergrund-ui</h2><br>"
                       "is a graphical frontend to create and modify configuration files for hintergrund.<br>"
                       "Licensed under the GPLv2. See <a href=\"http://www.gnu.org/license\">gnu.org</a><br>"
                       "created by <a href=\"https://github.com/univrsal\">univrsal</a><br><br>"
                       "Report issues at github.com/univrsal/hintergrund");
}

void MainWindow::on_actionQuit_triggered()
{
    quick_exit(0);
}

void MainWindow::on_actionView_Tags_triggered()
{
    auto* dialog = new tags_dialog(this);
    dialog->open();
}

void MainWindow::on_btn_add_tag_clicked()
{
    /* First remove any white spaces, since they're not allowed in tag names */
    auto text = ui->txt_new_label->text();
    ui->txt_new_label->setText(text.remove(' '));

    if (!m_selected_image) {
        QMessageBox::warning(this, "Error", "You must select a file first");
    } else if (ui->txt_new_label->text().length() < 1) {
        QMessageBox::warning(this, "Error", "You must provide a tag name");
    } else {
        auto* tmp = qPrintable(ui->txt_new_label->text());
        auto* tag = config::values.tag_manager->get_tag_for_str(tmp);
        if (tag) {
            if (m_selected_image->add_tag(tag)) {
                ui->list_tags->addItem(QString(tag->name()));
                ui->txt_new_label->clear();
            } else {
                QMessageBox::warning(this, "Error", "Duplicate tags.");
            }
        } else {
            QMessageBox::warning(this, "Error", "No existing tag with provided name. You can add new tags via Tools > Tags");
        }
    }
}

void MainWindow::on_file_tree_itemPressed(QTreeWidgetItem *item, int column)
{
    m_selected_image = nullptr;
    if (item->childCount() < 1) {
        /* this is a file */
        auto* f = ui_helper::image_tree_map[item];
        if (f) {
            f->path(m_selected_image_path);
            if (ui->actionShow_preview->isChecked())
                load_image(m_selected_image_path);
            ui->list_tags->clear();
            for (const auto& tag : f->additional_tags()) {
                ui->list_tags->addItem(QString(tag->name()));
            }
            m_selected_image = f;
        }
    }
}

void MainWindow::on_btn_remove_selected_clicked()
{
    auto selected = ui->list_tags->selectedItems();
    if (selected.empty()) {
        QMessageBox::warning(this, "Error", "No tag slected");
    } else {
        ui->list_tags->removeItemWidget(ui->list_tags->selectedItems().first());
    }
}

void MainWindow::on_actionSettings_triggered()
{
    auto* settings = new settings_dialog(this);
    settings->show();
}

void MainWindow::on_actionRules_triggered()
{
    auto* rules = new rules_dialog(this);
    rules->show();
}

void MainWindow::on_cb_fit_clicked()
{
    load_image(m_selected_image_path);
}

void MainWindow::on_actionShow_preview_triggered(bool checked)
{
    ui->lbl_image->setVisible(checked);
}
