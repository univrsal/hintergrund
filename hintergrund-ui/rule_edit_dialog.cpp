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

rule_edit_dialog::rule_edit_dialog(QWidget *parent, rule_type new_type) :
    QDialog(parent),
    ui(new Ui::rule_edit_dialog)
{
    ui->setupUi(this);
}

rule_edit_dialog::rule_edit_dialog(QWidget *parent, rule* edit_target) :
    QDialog(parent),
    ui(new Ui::rule_edit_dialog)
{
    ui->setupUi(this);
    m_edit_target = edit_target;
    std::vector<const tag*> active_tags;
    edit_target->get_tags(active_tags);
    /* All tags that apply to the rule which is currently begin edited
     * should be selected in the tag lits */
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

inline void date_to_qdate(const date_t* d, QDate& qd)
{
    qd.setDate(0, d->month, d->day);
}

inline void qdate_to_date(const QDate& qd, date_t* d)
{
    d->day = qd.day();
    d->month = static_cast<month_t>(qd.month());
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
        d_span = dynamic_cast<const rule_date_span*>(r);
        if (!d_span)
            break;
        date_to_qdate(d_span->begin(), d);
        ui->date_begin->setDate(d);
        if (d_span->is_span()) {
            date_to_qdate(d_span->end(), d);
            ui->date_end->setDate(d);
        }
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
    ui->txt_string_value_stdin->setEnabled(checked);
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
    copy_values();
    close();
}

void rule_edit_dialog::on_buttonBox_rejected()
{
    close();
}

inline month_t qmonth_to_month(int m) {
    return static_cast<month_t>(m + 1);
}

inline void qdate_to_date(const QDate& q, date_t& out)
{
    out.day = q.day();
    out.month =  qmonth_to_month(q.month());
}

inline void qtime_to_moment(const QTime& q, moment_t& out)
{
    out.hour = q.hour();
    out.minute = q.minute();
}

void rule_edit_dialog::copy_values()
{
    if (m_edit_target->type() == RULE_DATE) {
        auto* date = dynamic_cast<rule_date_span*>(m_edit_target);
        if (date) {
            date_t tmp;
            qdate_to_date(ui->date_begin->date(), tmp);
            date->set_is_span(ui->cb_date_span->isChecked());
            date->set_begin(tmp);
            qdate_to_date(ui->date_end->date(), tmp);
            date->set_end(tmp);
        }
    } else if (m_edit_target->type() == RULE_TIME) {
        auto* time = dynamic_cast<rule_time_span*>(m_edit_target);
        if (time) {
            moment_t tmp;
            qtime_to_moment(ui->time_begin->time(), tmp);
            time->set_begin(tmp);
            qtime_to_moment(ui->time_begin->time(), tmp);
            time->set_end(tmp);
        }
    } else if (m_edit_target->type() == RULE_MONTH) {
        auto* month = dynamic_cast<rule_month_span*>(m_edit_target);
        if (month) {
            date_t tmp;
            qdate_to_date(ui->date_month_begin->date(), tmp);
            month->set_is_span(ui->cb_month_span->isChecked());
            month->set_begin(tmp);
            qdate_to_date(ui->date_month_end->date(), tmp);
            month->set_end(tmp);
        }
    } else if (m_edit_target->type() == RULE_IO_FILE) {
        auto* file = dynamic_cast<rule_io_file*>(m_edit_target);
        if (file) {
            file->set_file_path(qPrintable(ui->txt_path->text()));
            if (ui->rb_check_number->isChecked()) {
                file->set_target(IO_INT);
                file->set_int_target(ui->sb_int_value->value());
                file->set_comp_type(static_cast<compare_type>
                                    (ui->cb_compare_type->currentIndex()));
            } else {
                file->set_target(IO_STRING);
                file->set_str_target(qPrintable(ui->txt_string_value->text()));
            }
        }
    } else if (m_edit_target->type() == RULE_IO_STDIN) {
        auto* in = dynamic_cast<rule_io_stdin*>(m_edit_target);
        if (in) {
            if (ui->rb_check_number_stdin->isChecked()) {
                in->set_target(IO_INT);
                in->set_comp_type(static_cast<compare_type>
                                    (ui->cb_compare_type_stdin->currentIndex()));
                in->set_int_target(ui->sb_int_value_stdin->value());
            } else {
                in->set_target(IO_STRING);
                in->set_str_target(qPrintable(ui->txt_string_value_stdin->text()));
            }
        }
    } else if (m_edit_target->type() == RULE_WEEKDAY) {
        auto* wd = dynamic_cast<rule_weekday*>(m_edit_target);
        if (wd) {
            wd->set_is_span(ui->cb_day_span->isChecked());
            weekday tmp;
            date_to_wd(tmp, ui->date_day_begin->date());
            wd->set_begin(tmp);
            date_to_wd(tmp, ui->date_day_end->date());
            wd->set_end(tmp);
        }
    }
}

void rule_edit_dialog::on_rule_edit_dialog_accepted()
{

}
