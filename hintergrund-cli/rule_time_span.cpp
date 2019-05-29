/* rule_time_span.cpp created on 2019.5.27
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
#include "rule_time_span.hpp"

rule_time_span::rule_time_span(moment_t start, moment_t end)
    : rule_date(TIME_SPAN)
{
    m_begin = start;
    m_end = end;
}

bool rule_time_span::evaluate()
{
    auto* t = now();
    if (t->tm_min >= m_begin.minute && t->tm_hour >= m_begin.hour &&
            t->tm_min <= m_end.minute && t->tm_hour <= m_end.hour)
    {
        return true;
    }
    return false;
}

bool rule_time_span::write_to_config(json_t* config)
{
    bool result = true;
    return result;
}

bool rule_time_span::read_from_config(json_t* config)
{
    bool result = true;

    return result;
}

