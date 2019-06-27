/* rule_time_span.hpp created on 2019.5.27
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
#include <stdint.h>

#define KEY_RULE_MOMENT_HOUR    "hour"
#define KEY_RULE_MOMENT_MINUTE  "minute"
#define KEY_RULE_TIME_BEGIN     "begin"
#define KEY_RULE_TIME_END       "end"

class rule_time_span : public rule_date
{
    moment_t m_begin, m_end;
public:
    rule_time_span();
    rule_time_span(moment_t start, moment_t end);

    bool evaluate() override;

    /* Encode/Decode this rule to the config file */
    bool write_to_config(json_t* config, json_error_t* error) override;
    bool read_from_config(json_t* config, json_error_t* error) override;

    const moment_t* begin() const;
    const moment_t* end() const;

    void set_begin(const moment_t& m);
    void set_end(const moment_t& m);

#ifdef HINTERGRUND_UI
    void to_string(QString& str) override
    {
        str += "Time rule | Start: ";
        str += QString::number(m_begin.hour).rightJustified(2, '0') + ":"
                + QString::number(m_begin.minute).rightJustified(2, '0');
        str += " - " + QString::number(m_end.hour).rightJustified(2, '0')
                + ":" + QString::number(m_end.minute).rightJustified(2, '0');
    }
#endif
};
