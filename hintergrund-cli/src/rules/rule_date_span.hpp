/* rule_date_span.hpp created on 2019.5.28
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

#define KEY_RULE_DATE_IS_SPAN   "is_span"
#define KEY_RULE_DATE_DAY       "day"
#define KEY_RULE_DATE_MONTH     "month"
#define KEY_RULE_DATE_BEGIN     "date_begin"
#define KEY_RULE_DATE_END       "date end"

class rule_date_span : public rule_date
{
    date_t m_start, m_end;
    bool m_is_span;
public:
    rule_date_span();
    rule_date_span(date_t start);
    rule_date_span(date_t start, date_t end);

    bool write_to_config(json_t* config, json_error_t* error) override;
    bool read_from_config(json_t* config, json_error_t* error) override;

    bool evaluate() override;
};

