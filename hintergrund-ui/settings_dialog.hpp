#pragma once

#include <QDialog>

namespace Ui {
class settings_dialog;
}

class settings_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit settings_dialog(QWidget *parent = nullptr);
    ~settings_dialog();

private slots:
    void on_settings_dialog_accepted();

private:
    Ui::settings_dialog *ui;
};

