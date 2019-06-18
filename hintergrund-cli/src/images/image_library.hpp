/* image_library.hpp created on 2019.5.29
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
#include "image.hpp"
#include "../folder.hpp"
#include <vector>
#include <memory>
#include <jansson.h>
#include <deque>

#define KEY_LIBRARY_SEQUENTIAL_INDEX    "index"
#define KEY_LIBRARY_BASE_FOLDER_ARRAY   "images"

class image_library
{
    std::vector<folder> m_base_folders;
    json_int_t m_sequential_current;
    bool m_loaded;
public:
    image_library();

    bool write_to_config(json_t* config, json_error_t* error);
    bool read_from_config(json_t *config, json_error_t *error);

    bool valid_file_type(const char* file_name);
    bool add_image(const char* file_name, std::deque<const tag*>& folder_tags);
    bool add_image(const char* file_naem, std::deque<const tag*>& folder_tags, std::deque<const tag*> add_tags);

    int image_count() const;
    bool loaded() const;
};

/* HEEEEEY, this is library */
namespace library {
    bool read_library(int* return_value);
}
