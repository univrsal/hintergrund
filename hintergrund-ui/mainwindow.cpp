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
#include "util/config.hpp"
#include "util/util.hpp"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    config::init_config();
}

MainWindow::~MainWindow()
{
    config::close_config();
    delete ui;
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

void MainWindow::on_file_tree_itemActivated(QTreeWidgetItem *item, int column)
{
    UNUSED_PARAM(item);
    UNUSED_PARAM(column);
}
