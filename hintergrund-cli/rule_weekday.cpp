/* rule_weekday.cpp created on 2019.5.27
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
#include "rule_weekday.hpp"

rule_weekday::rule_weekday()
    : rule_date(RULE_WEEKDAY)
{
    m_start = MONDAY;
    m_end = m_start;
    m_is_span = false;
}

rule_weekday::rule_weekday(weekday d)
    : rule_date(RULE_WEEKDAY)
{
    m_is_span = false;
    m_start = d;
}

rule_weekday::rule_weekday(weekday start, weekday end)
    : rule_date(RULE_WEEKDAY)
{
    m_is_span = true;
    /* Pick lower/higher for start/end incase arguments were passed wrong */
    m_start = start > end ? end : start;
    m_end = start > end ? start : end;
}

bool rule_weekday::evaluate()
{
    auto* n = now();
    if (m_is_span) {
        return n->tm_wday >= m_start && n->tm_wday <= m_end;
    } else {
        return n->tm_wday == m_start;
    }
}
