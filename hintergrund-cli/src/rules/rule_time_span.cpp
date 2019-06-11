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
#include "../util/util.hpp"

rule_time_span::rule_time_span()
    : rule_date(RULE_TIME)
{
    m_begin.hour = 0;
    m_begin.minute = 0;
    m_end = m_begin;
}

rule_time_span::rule_time_span(moment_t start, moment_t end)
    : rule_date(RULE_TIME)
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

bool rule_time_span::write_to_config(json_t* config, json_error_t* error)
{
    bool result = rule_date::write_to_config(config, error);

    if (result) {
        auto* start = json_pack_ex(error, 0, "{sisi}", KEY_RULE_MOMENT_HOUR, m_begin.hour,
                                   KEY_RULE_MOMENT_MINUTE, m_begin.minute);
        auto* end = json_pack_ex(error, 0, "{sisi}", KEY_RULE_MOMENT_HOUR, m_end.hour,
                                 KEY_RULE_MOMENT_MINUTE, m_end.minute);

        if (start && end) {
            json_object_set_new(config, KEY_RULE_TIME_BEGIN, start);
            json_object_set_new(config, KEY_RULE_TIME_END, end);
        } else {
            util::log("Error while packing time span data\n");
            result = false;
        }
    }
    return result;
}

bool rule_time_span::read_from_config(json_t* config, json_error_t* error)
{
    bool result = rule::read_from_config(config, error);

    if (result) {
        auto* start = json_object_get(config, KEY_RULE_TIME_BEGIN);
        auto* end = json_object_get(config, KEY_RULE_TIME_END);

        if (!start || json_unpack_ex(start, error, 0, "{sisi}", KEY_RULE_MOMENT_HOUR,
                                     &m_begin.hour, KEY_RULE_MOMENT_MINUTE, &m_begin.minute) < 0) {
            util::log("Error while decoding time span\n");
            result = false;
        }

        if (!end || json_unpack_ex(end, error, 0, "{sisi}", KEY_RULE_MOMENT_HOUR,
                                     &m_end.hour, KEY_RULE_MOMENT_MINUTE, &m_end.minute) < 0) {
            util::log("Error while decoding time span\n");
            result = false;
        }
    }

    return result;
}

