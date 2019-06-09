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
#include <unistd.h>

bool image_library::write_to_config(json_t *config, json_error_t *error)
{
    bool result = true;
    json_t* img_array = json_array();

    for (const auto& image : m_images) {
        if (!image->write_to_config(img_array, error)) {
            printf("Error while writing image to library array\n");
            util::print_json_error(error);
            result = false;
            break;
        }
    }

    if (result) {
        if (json_object_set_new(config, KEY_LIBRARY_SEQUENTIAL_INDEX, json_integer(m_sequential_current)) < 0 ||
                json_object_set_new(config, KEY_LIBRARY_IMAGE_ARRAY, img_array) < 0) {
            printf("Image library json value setting failed\n");
            result = false;
        }
    }
    return result;
}

bool image_library::read_from_config(json_t *config, json_error_t *error)
{
    bool result = true;
    size_t index;
    json_t* value;
    json_t* array = nullptr;

    if (json_unpack_ex(config, error, 0, "{siso}",
                       KEY_LIBRARY_SEQUENTIAL_INDEX, &m_sequential_current,
                       KEY_LIBRARY_IMAGE_ARRAY, array) < 0) {
        printf("Unpacking library object failed\n");
        util::print_json_error(error);
        result = false;
    } else  {
        json_array_foreach(array, index, value) {
            if (value) {
                auto* new_image = new image();
                if (!new_image->read_from_config(value, error)) {
                    printf("Error while reading image from library\n");
                    util::print_json_error(error);
                    delete new_image;
                    result = false;
                    break;
                } else {
                    m_images.emplace_back(new_image);
                }
            }
        }
    }
    json_decref(array);
    return result;
}

bool image_library::valid_file_type(const char *file_name)
{
    bool result = false;
    if (file_name) {
        const char* dot = strrchr(file_name, '.'); /* file.test.jpg -> .jpg */
        if (dot && strlen(++dot) > 0) { /* .jpg -> jpg */
            for (const auto& file_type : config::values.file_types) {
                if (strcmp(dot, file_type) == 0) {
                    result = true;
                    break;
                }
            }
        }
    }

    return result;
}

bool image_library::add_image(const char *file_name, std::deque<const tag *> &tags)
{
    bool result = true;
    char cwd[2049]; /* use current working directory as path */

    if (!getcwd(cwd, 2049)) {
        printf("Couldn't get current working directory\n");
        result = false;
    } else {
        result = add_image(file_name, cwd, tags);
    }

    return result;
}

bool image_library::add_image(const char *file_name, const char *path, std::deque<const tag *> &tags)
{
    if (!file_name || !path || strlen(file_name) < 1 || strlen(path) < 1)
        return false;
    char* full_path = nullptr;

    if (path[strlen(path)] != '/')
        full_path = util::concatenate(path, "/");
    else
        full_path = strdup(path);
    full_path = util::append(full_path, file_name);

    m_images.emplace_back(new image(full_path, tags));
    free((void*) full_path);

    return true;
}

const image_vector* image_library::images()
{
    return &m_images;
}

namespace library {
    bool read_library(int* return_value) {
        *return_value = config::SUCCESS;

        if (strlen(config::values.library_path) > 0) {
            json_error_t error;
            json_t* library_obj = nullptr;
            if (!util::file_exists(config::values.library_path)) {
                /* Create empty placeholder file */
                library_obj = json_object();
                if (!config::values.library->write_to_config(library_obj, &error)) {
                    printf("Creating empty library place holder failed\n");
                    *return_value = config::WRITE_EMPTY_LIBRARY_FAILED;
                }
            } else {
                library_obj = json_load_file(config::values.library_path, 0, &error);

                if (!library_obj || config::values.library->
                        read_from_config(library_obj, &error)) {
                     *return_value = config::READ_RULES_FAILED;
                }
            }
            if (library_obj)
                json_decref(library_obj);
        }
        return false;
    }
}
