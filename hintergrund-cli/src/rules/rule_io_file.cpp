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
#include "../util/util.hpp"
#include <cstring>
#include <fstream>

rule_io_file::rule_io_file()
    : rule_io(RULE_IO_FILE)
{
    m_file_path = nullptr;
}

rule_io_file::rule_io_file(const char* file_path, int target, compare_type ct)
    : rule_io(RULE_IO_FILE, target, ct)
{
    m_file_path = strdup(file_path);
}

rule_io_file::rule_io_file(const char* file_path, const char* target, compare_type ct)
    : rule_io(RULE_IO_FILE, target)
{
    m_file_path = strdup(file_path);
}

rule_io_file::~rule_io_file()
{
    free((void*)m_file_path);
}

bool rule_io_file::write_to_config(json_t *config, json_error_t *error)
{
    bool result = rule_io::write_to_config(config, error);

    if (result) {
        auto* file_path = json_pack_ex(error, 0, "s", m_file_path);
        if (!file_path || json_object_set_new(config, KEY_RULE_FILE_PATH, file_path) < 0) {
            debug("Error while packing io_rule file path\n");
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
        } else {
            debug("Error while getting io_rule file path\n");
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
                result = compare_int(i);
                break;
            case IO_STRING:
                if (std::getline(is, line))
                    result = line == m_str_target;
            break;
            default:;
        }
    }
    return result;
}

const char* rule_io_file::file_path() const
{
    return m_file_path;
}

void rule_io_file::set_file_path(const char *p)
{
    free((void*) m_file_path);
    m_file_path = strdup(p);
}
