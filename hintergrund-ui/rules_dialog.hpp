#pragma once

#include <QDialog>

namespace Ui {
class rules_dialog;
}

class rules_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit rules_dialog(QWidget *parent = nullptr);
    ~rules_dialog();

private slots:
    void on_btn_remove_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_btn_add_new_clicked();

private:
    Ui::rules_dialog *ui;
};

