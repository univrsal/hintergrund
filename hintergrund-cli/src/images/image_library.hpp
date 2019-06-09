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
#include <vector>
#include <memory>
#include <jansson.h>
#include <deque>

#define KEY_LIBRARY_SEQUENTIAL_INDEX    "index"
#define KEY_LIBRARY_IMAGE_ARRAY         "images"

typedef std::vector<std::unique_ptr<image>> image_vector;

class image_library
{
    image_vector m_images;
    uint32_t m_sequential_current = 0;
public:
    image_library() = default;

    bool write_to_config(json_t* config, json_error_t* error);
    bool read_from_config(json_t *config, json_error_t *error);

    bool valid_file_type(const char* file_name);
    bool add_image(const char* file_name, const char* path, std::deque<const tag*>& tags);
    bool add_image(const char* file_name, std::deque<const tag*>& tags);

    const image_vector* images();
};

/* HEEEEEY, this is library */
namespace library {
    bool read_library(int* return_value);
}
