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
#include "../util/util.hpp"

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

bool rule_weekday::write_to_config(json_t* config, json_error_t* error)
{
    bool result = rule_date::write_to_config(config, error);

    if (result) {
        auto* body = json_pack_ex(error, 0, "{sbsisi}", KEY_RULE_WEEKDAY_IS_SPAN,
                                  m_is_span, KEY_RULE_WEEKDAY_START, m_start,
                                  KEY_RULE_WEEKDAY_END, m_end);
        if (!body || json_object_set_new(config, KEY_RULE_WEEKDAY_BASE, body) < 0) {
            result = false;
            debug("Error while setting weekday base\n");
        }
    }

    return result;
}

bool rule_weekday::read_from_config(json_t *config, json_error_t *error)
{
    bool result = rule_date::read_from_config(config, error);

    if (result) {
        if (json_unpack_ex(config, error, 0, "{sbsisi}", KEY_RULE_WEEKDAY_IS_SPAN,
                           &m_is_span, KEY_RULE_WEEKDAY_START, &m_start,
                           KEY_RULE_WEEKDAY_END, &m_end) < 0) {
            result = false;
            debug("Error while unpacking weekday base\n");
        }
    }

    return result;
}
