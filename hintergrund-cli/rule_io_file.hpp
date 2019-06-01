/* rule_io_file.hpp created on 2019.5.29
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
#pragma once
#include "rule_io.hpp"

#define KEY_RULE_FILE_PATH      "file_path"

class rule_io_file : public rule_io
{
    const char* m_file_path;
public:
    rule_io_file(const char* file_path, int target, compare_type ct);
    rule_io_file(const char* file_path, const char* target, compare_type ct);

    ~rule_io_file();

    bool write_to_config(json_t* config, json_error_t* error) override;
    bool read_from_config(json_t* config, json_error_t* error) override;

    bool evaluate() override;
};
