/* rule_date.cpp created on 2019.5.27
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
#include "rule.hpp"
#include "../tagging/tag.hpp"
#include "../util/util.hpp"
#include "rule_date_span.hpp"
#include "rule_io_file.hpp"
#include "rule_io_stdin.hpp"
#include "rule_month_span.hpp"
#include "rule_time_span.hpp"
#include "rule_weekday.hpp"
#include <sstream>

rule::rule(rule_type type)
{
    m_type = type;
}

void rule::get_tags(std::vector<const tag*>& tags)
{
    for (auto tag : m_tags)
        tags.emplace_back(tag);
}

int rule::priority()
{
    return m_priority;
}

bool rule::write_to_config(json_t* json, json_error_t* error)
{
    /* Packs basic member variables */
    bool result = true;

    auto* j_base = json_pack_ex(error, 0, "{sisi}", KEY_RULE_TYPE, m_type, KEY_RULE_PRIORITY,
                             m_priority);
    if (!j_base || json_object_set_new(json, KEY_RULE_BASE, j_base) < 0) {
        result = false;
        debug("Error while writing rule to file\n");
    }

    auto* tag_array = json_array();

    if (!tag_array) {
        debug("Error while creating tag array\n");
        result = false;
    } else if (result) {
        for (const auto& tag : m_tags) {
            if (!tag->write_to_config(tag_array, error)) {
                debug("Error while writing rule to file\n");
                result = false;
                break;
            }
        }

        if (result && json_object_set_new(json, KEY_RULE_TAGS, tag_array) < 0) {
            debug("Error while settings tag array\n");
            result = false;
        }
    }

    return result;
}

bool rule::read_from_config(json_t* config, json_error_t* error)
{
    bool result = true;

    auto* base = json_object_get(config, KEY_RULE_BASE);
    if (base) {
        if (json_unpack_ex(base, error, 0, "{sisi}", KEY_RULE_TYPE,
                           &m_type, KEY_RULE_PRIORITY, &m_priority) < 0) {
            result = false;
            debug("Error while unpacking base info about rule\n");
        }
    } else {
        debug("Error getting base values for rule\n");
    }

    auto* tag_array = json_object_get(config, KEY_RULE_TAGS);
    if (result && tag_array) {
        size_t index;
        json_t* value = nullptr;
        json_array_foreach(tag_array, index, value) {
            auto* new_tag = new tag();
            result = new_tag->read_from_config(value, error);
            if (!result)
                break;
        }
    }
    return result;
}

rule_type rule::type() const
{
    return m_type;
}

rule* rule::make(rule_type t)
{
    switch (t) {
    case RULE_DATE:
        return new rule_date_span;
    case RULE_TIME:
        return new rule_time_span;
    case RULE_MONTH:
        return new rule_month_span;
    case RULE_IO_FILE:
        return new rule_io_file;
    case RULE_IO_STDIN:
        return new rule_io_stdin;
    case RULE_WEEKDAY:
        return new rule_weekday;
    }
    return nullptr;
}
