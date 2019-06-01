/* rule_month_span.cpp created on 2019.5.28
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
#include "rule_month_span.hpp"

rule_month_span::rule_month_span()
    : rule_date(RULE_MONTH)
{
    m_start.day = 0;
    m_start.month = JANUARY;
    m_end = m_start;
    m_is_span = false;
}

rule_month_span::rule_month_span(date_t start)
    : rule_date(RULE_MONTH)
{
    m_start = start;
    m_is_span = false;
}

rule_month_span::rule_month_span(date_t start, date_t end)
    : rule_date(RULE_MONTH)
{
    m_start = start;
    m_end = end;
    m_is_span = true;
}

bool rule_month_span::evaluate()
{
    auto* n = now();

    if (m_is_span) {
        return n->tm_mon >= m_start.month && n->tm_mon <= m_end.month;
    } else {
        return n->tm_mon == m_start.month;
    }
}
