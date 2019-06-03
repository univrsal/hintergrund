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

rule_set::rule_set()
{

}

bool rule_set::write_rules(json_t *json, json_error_t *error)
{
    auto* rule_array = json_array();
    bool result = true;

    if (!rule_array) {
        printf("Error creating rule array\n");
        return false;
    }

    for (const auto& rule : m_rules) {
        auto* rule_entry = json_object();
        if (!rule->write_to_config(rule_entry, error)) {
            result = false;
            break;
        } else {
            if (json_array_append_new(rule_array, rule_entry) < 0) {
                printf("Error while appending to rule aray\n");
                result = false;
                break;
            }
        }
    }

    if (result && json_object_set_new(json, KEY_RULE_ARRAY, rule_array) < 0) {
        result = false;
        printf("Error while setting rule array\n");
    }

    return result;
}

bool rule_set::read_rules(json_t *json, json_error_t *error)
{
    bool result = true;

    auto* rule_array = json_object_get(json, KEY_RULE_ARRAY);
    rule* new_rule = nullptr;

    if (rule_array) {
        size_t index;
        json_t* value = nullptr;
        json_array_foreach(rule_array, index, value) {
            if (value) {
                auto* base_info = json_object_get(value, KEY_RULE_BASE);
                if (base_info) {
                    auto* type_id = json_object_get(base_info, KEY_RULE_TYPE);
                    json_decref(base_info);

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
                            case RULE_IO_STDIN: /* TODO */
                                break;
                        }
                        json_decref(type_id);
                        if (new_rule && !new_rule->read_from_config(value, error)) {
                            result = false;
                            break;
                        }
                    } else {
                        printf("Error while loading rule type\n");
                        result = false;
                        break;
                    }
                } else {
                    printf("Error while loading rules array\n");
                    result = false;
                    break;
                }
            }
        }
        json_decref(rule_array);
    }
    return result;
}
