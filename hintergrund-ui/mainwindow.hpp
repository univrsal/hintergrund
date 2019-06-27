/* mainwindow.hpp created on 2019.5.28
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
#pragma once

#include <QMainWindow>
#include <QTreeWidgetItem>
namespace Ui {
class MainWindow;
}
class image;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionAbout_triggered();

    void on_actionQuit_triggered();

    void on_actionView_Tags_triggered();

    void on_btn_add_tag_clicked();

    void on_file_tree_itemPressed(QTreeWidgetItem *item, int column);

    void on_btn_remove_selected_clicked();

    void on_actionSettings_triggered();

    void on_actionRules_triggered();

private:
    bool load_image(const QString& path);

    Ui::MainWindow *ui;
    image* m_selected_image;
};

