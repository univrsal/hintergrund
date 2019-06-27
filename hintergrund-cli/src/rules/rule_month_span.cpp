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
#include "../util/util.hpp"

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

bool rule_month_span::write_to_config(json_t* config, json_error_t* error)
{
    bool result = rule_date::write_to_config(config, error);

    if (result) {
        auto start = json_pack_ex(error, 0, "{sisi}", KEY_RULE_MONTH_SPAN_MONTH, m_start.month, KEY_RULE_MONTH_SPAN_DAY,
                                  m_start.day);
        if (!start || json_object_set_new(config, KEY_RULE_MONTH_SPAN_START, start) < 0) {
            result = false;
            debug("Error while setting date start\n");
        }

        auto span = json_pack_ex(error, 0, "b", m_is_span);
        if (!span || json_object_set_new(config, KEY_RULE_MONTH_SPAN_IS_SPAN, span) < 0) {
            debug("Error while setting is_span boolean\n");
            result = false;
        }

        if (m_is_span) {
            auto end = json_pack_ex(error, 0, "{sisi}", KEY_RULE_MONTH_SPAN_MONTH, m_end.month, KEY_RULE_MONTH_SPAN_DAY, m_end.day);
            if (end && json_object_set_new(config, KEY_RULE_MONTH_SPAN_END, end) < 0) {
                debug("Error while setting date end\n");
                result = false;
            }
        }
    }
    return result;
}

bool rule_month_span::read_from_config(json_t* config, json_error_t* error)
{
    bool result = rule::read_from_config(config, error);
    if (result) {
        auto* start = json_object_get(config, KEY_RULE_MONTH_SPAN_START);
        auto* span = json_object_get(config, KEY_RULE_MONTH_SPAN_IS_SPAN);

        if (!start || json_unpack_ex(start, error, 0, "{sisi}",
                                     KEY_RULE_MONTH_SPAN_MONTH, &m_start.month,
                                     KEY_RULE_MONTH_SPAN_DAY, &m_start.day) < 0)
        {
            debug("Error while decoding month span start\n");
            result = false;
        } else if (!span || json_unpack_ex(span, error, 0, "b",
                                           KEY_RULE_MONTH_SPAN_IS_SPAN, &m_is_span)) {
            debug("Error while decoding month span is span\n");
            result = false;
        } else if (m_is_span) {
            auto* end = json_object_get(config, KEY_RULE_MONTH_SPAN_END);
            if (!end || json_unpack_ex(end, error, 0, KEY_RULE_MONTH_SPAN_MONTH,
                                       &m_end.month, KEY_RULE_MONTH_SPAN_DAY,
                                       &m_end.day) < 0) {
                debug("Error while decoding month span end\n");
                result = false;
            }
        }
    }

    return result;
}

const date_t* rule_month_span::begin() const
{
    return &m_start;
}

const date_t* rule_month_span::end() const
{
    return &m_end;
}

bool rule_month_span::is_span() const
{
    return m_is_span;
}

void rule_month_span::set_begin(const date_t &d)
{
    m_start = d;
}

void rule_month_span::set_end(const date_t &d)
{
    m_end = d;
}

void rule_month_span::set_is_span(bool b)
{
    m_is_span = b;
}
