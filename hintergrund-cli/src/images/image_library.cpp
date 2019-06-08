/* image_library.cpp created on 2019.5.29
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
#include "image_library.hpp"
#include "../util/util.hpp"
#include "../util/config.hpp"
#include <cstring>

bool image_library::write_to_config(json_t *config, json_error_t *error)
{
    bool result = true;
    for (const auto& image : m_images) {
        if (!image->write_to_config(config, error)) {
            printf("Error while writing image to library array\n");
            util::print_json_error(error);
            result = false;
            break;
        }
    }
    return result;
}

bool image_library::read_from_config(json_t *config, json_error_t *error)
{
    bool result = true;
    size_t index;
    json_t* value;

    json_array_foreach(config, index, value) {
        if (value) {
            auto* new_image = new image();
            if (!new_image->read_from_config(value, error)) {
                printf("Erro while reading image from library\n");
                util::print_json_error(error);
                delete new_image;
                result = false;
                break;
            } else {
                m_images.emplace_back(new_image);
            }
        }
    }
    return result;
}

namespace library {
    bool read_library(int* return_value) {
        bool result = true;
        *return_value = config::SUCCESS;

        if (strlen(config::values.library_path) > 0) {
            if (!util::file_exists(config::values.library_path)) {
                /* Create empty placeholder file */
                if (!util::try_create_file(config::values.library_path))
                    printf("Failed to create placeholder library file."
                           "Make sure the permissions are set correctly\n");
            } else {
                json_error_t error;
                json_t* library_array = json_load_file(config::values.library_path, 0, &error);

                if (!library_array || config::values.library->
                        read_from_config(library_array, &error)) {
                    result = false;
                     *return_value = config::READ_RULES_FAILED;
                }
            }
        }
        return result;
    }
}
