/* rule_io.cpp created on 2019.5.28
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
#include "rule_io.hpp"
#include "../util/util.hpp"
#include <cstring>

rule_io::rule_io(rule_type type)
    : rule(type)
{
    m_io_type = IO_INVALID;
    m_int_target = 0;
    m_str_target = nullptr;
    m_comp_type = COMP_INVALID;
}

rule_io::rule_io(rule_type type, int32_t i, compare_type ct)
    : rule(type)
{
    m_int_target = i;
    m_str_target = nullptr;
    m_io_type = IO_INT;
    m_comp_type = ct;
}

rule_io::rule_io(rule_type type, const char* str)
    : rule(type)
{
    m_str_target = strdup(str);
    m_io_type = IO_STRING;
    m_comp_type = COMP_EQUAL; /* Only compare strings with == */
}

bool rule_io::write_to_config(json_t *config, json_error_t *error)
{
    bool result = rule::write_to_config(config, error);

    if (result) {
        if (m_io_type == IO_INT) {
            auto* base_vals = json_pack_ex(error, 0, "{sisisi}", KEY_RULE_IO_TYPE,
                                           m_io_type, KEY_RULE_COMP_TYPE, m_comp_type,
                                           KEY_RULE_INT_TARGET, m_int_target);
            if (!base_vals || json_object_set_new(config, KEY_RULE_IO_BASE, base_vals) < 0) {
                util::log("Error while packing base values for io_rule\n");
                result = false;
            }
        } else {
            auto* base_vals = json_pack_ex(error, 0, "{sisiss}", KEY_RULE_IO_TYPE,
                                           m_io_type, KEY_RULE_COMP_TYPE, m_comp_type,
                                           KEY_RULE_STRING_TARGET, m_str_target);
            if (!base_vals || json_object_set_new(config, KEY_RULE_IO_BASE, base_vals) < 0) {
                util::log("Error while packing base values for io_rule\n");
                result = false;
            }
        }
    }

    return result;
}

bool rule_io::read_from_config(json_t *config, json_error_t *error)
{
    bool result = rule::read_from_config(config, error);

    if (result) {
        auto* base_vals = json_object_get(config, KEY_RULE_IO_BASE);

        if (base_vals) {
            auto* io_type = json_object_get(base_vals, KEY_RULE_IO_TYPE);
            int unpack_result = -1;

            if (io_type) {
                if (json_integer_value(io_type) == IO_INT) {
                    unpack_result = json_unpack_ex(base_vals, error, 0, "{sisisi}", KEY_RULE_IO_TYPE,
                                                   &m_io_type, KEY_RULE_COMP_TYPE, &m_comp_type,
                                                   KEY_RULE_INT_TARGET, &m_int_target);
                } else {
                    unpack_result = json_unpack_ex(base_vals, error, 0, "{sisiss}", KEY_RULE_IO_TYPE,
                                                   &m_io_type, KEY_RULE_COMP_TYPE, &m_comp_type,
                                                   KEY_RULE_STRING_TARGET, &m_str_target);
                }
            }

            if (unpack_result < 0) {
                result = false;
                util::log("Error while unpacking rule_io base values\n");
            }
        } else {
            result = false;
            util::log("Error while getting base rule_io data\n");
        }
    }

    return result;
}

rule_io::~rule_io()
{
    delete m_str_target;
}
