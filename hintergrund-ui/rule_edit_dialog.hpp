#pragma once

#include <QDialog>
#include "rules/rule.hpp"

namespace Ui {
class rule_edit_dialog;
}

class rule_edit_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit rule_edit_dialog(QWidget *parent = nullptr);
    rule_edit_dialog(QWidget *parent, rule* edit_target);
    ~rule_edit_dialog();

    void select_rule_tab(rule_type type);

    void load_rule(const rule* r);
private slots:
    void on_rb_check_string_toggled(bool checked);

    void on_rb_check_string_2_toggled(bool checked);

    void on_cb_day_span_toggled(bool checked);

    void on_cb_date_span_toggled(bool checked);

    void on_cb_month_span_toggled(bool checked);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    rule* m_edit_target = nullptr;
    Ui::rule_edit_dialog *ui;
};

