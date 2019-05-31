/* rule_date_span.cpp created on 2019.5.28
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
#include "rule_date_span.hpp"

rule_date_span::rule_date_span(date_t start)
    : rule_date(RULE_DATE)
{
    m_start = start;
    m_is_span = false;
}

rule_date_span::rule_date_span(date_t start, date_t end)
    : rule_date(RULE_DATE)
{
    m_start = start;
    m_end = end;
    m_is_span = true;
}

bool rule_date_span::evaluate()
{
    auto* n = now();

    if (m_is_span) {
        return n->tm_mon >= m_start.month && n->tm_mon <= m_end.month &&
               n->tm_mday >= m_start.day && n->tm_mday <= m_end.day;
    } else {
        return n->tm_mday == m_start.day && n->tm_mon == m_start.month;
    }
}

bool rule_date_span::write_to_config(json_t* config, json_error_t* error)
{
    bool result = rule::write_to_config(config, error);

    if (result) {
        auto start = json_pack_ex(error, 0, "{sisi}", KEY_RULE_DATE_MONTH,
                                  m_start.month, KEY_RULE_DATE_DAY, m_start.day);
        auto end = json_pack_ex(error, 0, "{sisi}", KEY_RULE_DATE_MONTH,
                                  m_end.month, KEY_RULE_DATE_DAY, m_end.day);

        if (start && end) {
            json_object_set_new(config, KEY_RULE_DATE_BEGIN, start);
            json_object_set_new(config, KEY_RULE_DATE_END, end);
        } else {
            printf("Error while creating json values for date span\n");
            result = false;
        }
    }
    return result;
}

bool rule_date_span::read_from_config(json_t* config, json_error_t* error)
{
    bool result = rule::read_from_config(config, error);

    if (result) {
        auto* start = json_object_get(config, KEY_RULE_DATE_BEGIN);
        auto* end = json_object_get(config, KEY_RULE_DATE_END);

        if (!start || json_unpack_ex(start, error, 0, "{sisi}", KEY_RULE_DATE_MONTH,
                                     &m_start.month, KEY_RULE_DATE_DAY, &m_start.day) < 0) {
            printf("Error while decoding date\n");
            result = false;
        } else {
            json_decref(start);
        }

        if (!end || json_unpack_ex(end, error, 0, "{sisi}", KEY_RULE_DATE_MONTH,
                                   &m_end.month, KEY_RULE_DATE_DAY, &m_end.month) < 0){
            printf("Error while decoding date\n");
            result = false;
        } else {
            json_decref(end);
        }
    }

    return result;
}
