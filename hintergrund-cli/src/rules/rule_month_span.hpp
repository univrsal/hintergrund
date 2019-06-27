/* rule_month_span.hpp created on 2019.5.28
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
#include "rule_date.hpp"

#define KEY_RULE_MONTH_SPAN_IS_SPAN   "is_span"
#define KEY_RULE_MONTH_SPAN_START     "start"
#define KEY_RULE_MONTH_SPAN_END       "end"
#define KEY_RULE_MONTH_SPAN_MONTH     "month"
#define KEY_RULE_MONTH_SPAN_DAY       "day"

class rule_month_span : public rule_date
{
    date_t m_start, m_end;
    bool m_is_span;
public:
    rule_month_span();
    rule_month_span(date_t start);
    rule_month_span(date_t start, date_t end);

    bool write_to_config(json_t* config, json_error_t* error) override;

    bool read_from_config(json_t* config, json_error_t* error) override;

    bool evaluate() override;

    const date_t* begin() const;
    const date_t* end() const;
    bool is_span() const;

    void set_is_span(bool b);
    void set_begin(const date_t& d);
    void set_end(const date_t& d);

#ifdef HINTERGRUND_UI
    void to_string(QString& str) override
    {
        str += "Month rule | ";
        if (m_is_span)
            str += "Start: ";
        str += month_to_str(m_start.month);

        if (m_is_span) {
            str += "End: ";
            str += month_to_str(m_end.month);
        }
    }
#endif
};

