/* rule_weekday.hpp created on 2019.5.27
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

#define KEY_RULE_WEEKDAY_IS_SPAN    "is_span"
#define KEY_RULE_WEEKDAY_START      "start"
#define KEY_RULE_WEEKDAY_END        "end"

enum weekday {
    SUNDAY,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

class rule_weekday : public rule_date
{
    bool m_is_span;
    weekday m_start, m_end;
public:
    rule_weekday();
    rule_weekday(weekday d);
    rule_weekday(weekday start, weekday end);

    bool write_to_config(json_t* config, json_error_t* error) override;

    bool read_from_config(json_t* config, json_error_t* error) override;

    bool evaluate() override;
};

