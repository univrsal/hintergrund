#pragma once

#include <QDialog>

namespace Ui {
class tags_dialog;
}

class tags_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit tags_dialog(QWidget *parent = nullptr);
    ~tags_dialog();

private slots:

    void on_btn_remove_selected_clicked();

    void on_tags_table_cellClicked(int row, int column);

    void on_btn_save_clicked();

    void on_btn_save_new_clicked();

    void on_buttonBox_accepted();

private:
    Ui::tags_dialog *ui;
};

