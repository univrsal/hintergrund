/* image.hpp created on 2019.6.5
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
#include <vector>
#include <jansson.h>

class tag;

class image
{
    const char* m_path; /* Full file path on hdd */
    std::vector<tag*> m_tags;
public:
    image();

    bool write_to_config(json_t* config, json_error_t* error);
    bool read_from_config(json_t* config, json_error_t* error);
};
