#pragma once

#include <QDialog>

namespace Ui {
class settings_dialog;
}
class QLineEdit;

class settings_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit settings_dialog(QWidget *parent = nullptr);
    ~settings_dialog();

private slots:
    void on_settings_dialog_accepted();

    void on_btn_lib_path_clicked();

    void on_btn_tag_path_clicked();

    void on_btn_rule_path_clicked();

    void on_btn_add_type_clicked();

    void on_btn_remove_selected_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    void browse_file(QLineEdit* le, const QString& title);
    Ui::settings_dialog *ui;
};

