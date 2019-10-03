#pragma once

#include <QDialog>
#include <QMap>

namespace Ui {
class rules_dialog;
}

class rule;

class rules_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit rules_dialog(QWidget *parent = nullptr);
    ~rules_dialog();

    void refresh_list();

    void add_rule(rule* r);

private slots:
    void on_btn_remove_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_btn_add_new_clicked();

    void on_btn_edit_clicked();

private:
    Ui::rules_dialog *ui;
    QMap<int, rule*> m_rule_map;
};

