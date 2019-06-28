#include "rule_edit_dialog.hpp"
#include "ui_rule_edit_dialog.h"
#include "util/config.hpp"
#include "tagging/tagger.hpp"
#include "rules/rule_date_span.hpp"
#include "rules/rule_month_span.hpp"
#include "rules/rule_io_file.hpp"
#include "rules/rule_io_stdin.hpp"
#include "rules/rule_time_span.hpp"
#include "rules/rule_weekday.hpp"
#include <vector>

rule_edit_dialog::rule_edit_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rule_edit_dialog)
{
    ui->setupUi(this);
    for (const auto& tag : config::values.tag_manager->tags())
    {
        ui->list_tags_used->addItem(tag->name());
    }
}

rule_edit_dialog::rule_edit_dialog(QWidget *parent, rule* edit_target) :
    QDialog(parent),
    ui(new Ui::rule_edit_dialog)
{
    ui->setupUi(this);
    m_edit_target = edit_target;
    std::vector<const tag*> active_tags;
    edit_target->get_tags(active_tags);

    for (const auto& tag : config::values.tag_manager->tags())
    {
        auto* new_item = new QListWidgetItem(tag->name());
        for (const auto* atag : active_tags) {
            if (atag->id() == tag->id()) {
                /* This rule uses this tag -> Select it in the rule list*/
                new_item->setSelected(true);
                break;
            }
        }
        ui->list_tags_used->addItem(tag->name());
    }
    select_rule_tab(edit_target->type());
    load_rule(edit_target);
}

rule_edit_dialog::~rule_edit_dialog()
{
    delete ui;
}

void rule_edit_dialog::select_rule_tab(rule_type type)
{
    /* Disable all other widgets to prevent user
     * from switching tabs */
    for (int i = 0; i < RULE_COUNT; i++) {
        if (i != type) {
            ui->tabs_rule_types->widget(i)->setEnabled(false);
        }
    }
    ui->tabs_rule_types->setCurrentIndex(type);
}


void weekday_to_date(weekday wd, QDate& d)
{
    /* Qt doesn't offer a way to set the weekday
     * so we first take the current date, reverse until
     * weekday is sunday and then add the weekday value of
     * the weekday rule. Sunday is zero, so adding won't
     * change anything
     */
    d = QDate::currentDate();
    while (d.dayOfWeek() != 7) /* In QDate Sunday = 7, Monday = 0 */
        d.setDate(d.year(), d.month(), d.day() - 1);
    d.setDate(d.year(), d.month(), d.day() + wd);
}

inline void date_to_wd(weekday& wd, const QDate& d)
{
    if (d.dayOfWeek() == 7)
        wd = SUNDAY;
    else
        wd = static_cast<weekday>(d.dayOfWeek());
}

void rule_edit_dialog::load_rule(const rule *r)
{
    const rule_time_span* t_span = nullptr;
    const rule_weekday* wd = nullptr;
    const rule_date_span* d_span = nullptr;
    QTime t;
    QDate d;

    switch (r->type()) {
    case RULE_TIME:
        t_span = dynamic_cast<const rule_time_span*>(r);
        if (!t_span)
            break;
        t = QTime(t_span->begin()->hour, t_span->begin()->minute, t_span->begin()->minute);
        ui->time_begin->setTime(t);
        t = QTime(t_span->end()->hour, t_span->end()->minute, t_span->end()->minute);
        ui->time_end->setTime(t);
        break;
    case RULE_WEEKDAY:
        wd = dynamic_cast<const rule_weekday*>(r);
        if (!wd)
            break;
        weekday_to_date(wd->begin(), d);
        ui->date_day_begin->setDate(d);
        if (wd->is_span()) {
            weekday_to_date(wd->end(), d);
            ui->date_day_end->setDate(d);
        }
        break;
    case RULE_DATE:
        break;
    }
}

void rule_edit_dialog::on_rb_check_string_toggled(bool checked)
{
    ui->txt_string_value->setEnabled(checked);
    ui->frame_check_number->setEnabled(!checked);
}

void rule_edit_dialog::on_rb_check_string_2_toggled(bool checked)
{
    ui->txt_string_value_2->setEnabled(checked);
    ui->frame_check_number2->setEnabled(!checked);
}

void rule_edit_dialog::on_cb_day_span_toggled(bool checked)
{
    ui->lbl_end_day->setEnabled(checked);
    ui->date_day_end->setEnabled(checked);
}

void rule_edit_dialog::on_cb_date_span_toggled(bool checked)
{
    ui->lbl_end_date->setEnabled(checked);
    ui->date_end->setEnabled(checked);
}

void rule_edit_dialog::on_cb_month_span_toggled(bool checked)
{
    ui->lbl_month_end->setEnabled(checked);
    ui->date_month_end->setEnabled(checked);
}

void rule_edit_dialog::on_buttonBox_accepted()
{
    close();
}

void rule_edit_dialog::on_buttonBox_rejected()
{
    close();
}
