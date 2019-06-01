/* rule_io_file.cpp created on 2019.5.29
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
#include "rule_io_file.hpp"
#include <cstring>
#include <fstream>

rule_io_file::rule_io_file(const char* file_path, int target, compare_type ct)
    : rule_io(RULE_IO_FILE, target)
{
    m_comp_type = ct;
    m_file_path = strdup(file_path);
}

rule_io_file::rule_io_file(const char* file_path, const char* target, compare_type ct)
    : rule_io(RULE_IO_FILE, target)
{
    m_comp_type = ct;
    m_file_path = strdup(file_path);
}

rule_io_file::~rule_io_file()
{
    delete m_file_path;
}

bool rule_io_file::write_to_config(json_t *config, json_error_t *error)
{
    bool result = rule_io::write_to_config(config, error);

    if (result) {
        auto* file_path = json_pack_ex(error, 0, "s", m_file_path);
        if (file_path) {
            json_object_set_new(config, KEY_RULE_FILE_PATH, file_path);
        } else {
            printf("Error while packing io_rule file path\n");
            result = false;
        }
    }

    return result;
}

bool rule_io_file::read_from_config(json_t *config, json_error_t *error)
{
    bool result = rule_io::read_from_config(config, error);

    if (result) {
        auto* file_path = json_object_get(config, KEY_RULE_FILE_PATH);

        if (file_path) {
            m_file_path = strdup(json_string_value(file_path));
            json_decref(file_path);
        } else {
            printf("Error while getting io_rule file path\n");
            result = false;
        }
    }

    return result;
}

bool rule_io_file::evaluate()
{
    std::ifstream is(m_file_path);
    bool result = false;
    if (is.good())
    {
        int i;
        std::string line;
        switch (m_io_type) {
            case IO_INT:
                is >> i;
                switch (m_comp_type) {
                    case COMP_EQUAL:
                        result = i == m_int_target;
                        break;
                    case COMP_LESS_THAN:
                        result = i < m_int_target;
                        break;
                    case COMP_GREATER_THAN:
                        result = i > m_int_target;
                        break;
                    case COMP_GREATER_EQ_THAN:
                        result = i >= m_int_target;
                        break;
                    case COMP_LESS_EQ_THAN:
                        result = i <= m_int_target;
                        break;
                }
                break;
            case IO_STRING:
                if (std::getline(is, line))
                    result = line == m_str_target;
            break;
        }
    }
    return result;
}
