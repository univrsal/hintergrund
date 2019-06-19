/* rule_set.cpp created on 2019.5.29
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
#include "rule_set.hpp"
#include "rule_date_span.hpp"
#include "rule_month_span.hpp"
#include "rule_weekday.hpp"
#include "rule_io_file.hpp"
#include "rule_time_span.hpp"
#include "rule_io_stdin.hpp"
#include "../util/util.hpp"
#include "../util/config.hpp"
#include <algorithm>
#include <cstring>

rule_set::rule_set()
{
    m_loaded = false;
}

void rule_set::get_active_tags(std::vector<const tag*>& tagv)
{
    /* sort by priority first (highest to lowest) */
    std::sort(m_rules.rbegin(), m_rules.rend());
    int highest_priority = -1; /* priority of highest active rule */
    for (const auto& rule : m_rules) {
        /* If a priority was evaluated and the current rule is ranked lower,
           the loop can exit since all following rules are also ranked lower */
        if (rule->priority() < highest_priority)
            break;
        if (rule->evaluate()) {
            highest_priority = rule->priority();
            rule->get_tags(tagv);
        }
    }
}

bool rule_set::write_rules(json_t *json, json_error_t *error)
{
    auto* rule_array = json_array();
    bool result = true;

    if (!rule_array) {
        debug("Error creating rule array\n");
        return false;
    }

    for (const auto& rule : m_rules) {
        auto* rule_entry = json_object();
        if (!rule->write_to_config(rule_entry, error)) {
            result = false;
            break;
        } else {
            if (json_array_append_new(rule_array, rule_entry) < 0) {
                debug("Error while appending to rule aray\n");
                result = false;
                break;
            }
        }
    }

    if (result && json_object_set_new(json, KEY_RULE_ARRAY, rule_array) < 0) {
        result = false;
        debug("Error while setting rule array\n");
    }

    return result;
}

bool rule_set::read_rules(const char* path)
{
    bool result = true;

    json_error_t error;
    json_t* rule_array = json_load_file(path, 0, &error);
    rule* new_rule = nullptr;

    if (rule_array) {
        size_t index;
        json_t* value = nullptr;
        json_array_foreach(rule_array, index, value) {
            auto* base_info = json_object_get(value, KEY_RULE_BASE);
            if (base_info) {
                auto* type_id = json_object_get(base_info, KEY_RULE_TYPE);

                if (type_id) {
                    switch (json_integer_value(type_id)) {
                        case RULE_DATE:
                            new_rule = new rule_date_span();
                            break;
                        case RULE_TIME:
                            new_rule = new rule_time_span();
                            break;
                        case RULE_WEEKDAY:
                            new_rule = new rule_weekday();
                            break;
                        case RULE_MONTH:
                            new_rule = new rule_month_span();
                            break;
                        case RULE_IO_FILE:
                            new_rule = new rule_io_file();
                            break;
                        case RULE_IO_STDIN:
                            new_rule = new rule_io_stdin();
                            break;
                    }
                    if (new_rule && !new_rule->read_from_config(value, &error)) {
                        result = false;
                        util::print_json_error(&error);
                        break;
                    }
                } else {
                    debug("Error while loading rule type\n");
                    result = false;
                    break;
                }
            } else {
                debug("Error while loading rule base info\n");
                result = false;
                break;
            }
        }
        json_decref(rule_array);
    } else {
        debug("Error loading rule array\n");
        util::print_json_error(&error);
    }
    m_loaded = result;
    return result;
}

int rule_set::rule_count() const
{
    return m_rules.size();
}

bool rule_set::loaded() const
{
    return m_loaded;
}

namespace rules {
    bool read_rules(int* return_value)
    {
        *return_value = config::SUCCESS;
        if (config::values.rules_manager->loaded())
            return false;

        if (strlen(config::values.rule_path) < 1) {
            *return_value = config::MISSING_ARG;
            debug("Missing rules path\n");
            return false;
        }

        if (util::file_exists(config::values.rule_path)) {
            if (config::values.rules_manager->read_rules(
                        config::values.rule_path)) {
                debug("Successfully loaded %i rules\n",
                          config::values.rules_manager->rule_count());
            } else {
                *return_value = config::READ_RULES_FAILED;
            }
         } else {
            /* Create empty placeholder file */
            json_t* arr = json_array();
            if (json_dump_file(arr, config::values.rule_path, 0) < 0) {
                debug("Failed to create placeholder rule file."
                       "Make sure the permissions are set correctly\n");
            } else {
                debug("Successfully created default rules file\n");
            }
            json_decref(arr);
         }

         return false;
     }
}
