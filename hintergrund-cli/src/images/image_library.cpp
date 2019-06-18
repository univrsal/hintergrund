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

image_library::image_library()
{
    m_sequential_current = 0;
    m_loaded = false;
}

bool image_library::write_to_config(json_t *config, json_error_t *error)
{
    bool result = true;
    json_t* img_array = json_array();

    for (const auto& folder : m_base_folders) {
        if (!folder->write_to_config(img_array, error)) {
            debug("Error while writing base folder to json\n");
            util::print_json_error(error);
            result = false;
            break;
        }
    }

    if (result) {
        if (json_object_set_new(config, KEY_LIBRARY_SEQUENTIAL_INDEX, json_integer(m_sequential_current)) < 0 ||
                json_object_set_new(config, KEY_LIBRARY_BASE_FOLDER_ARRAY, img_array) < 0) {
             debug("Image library json value setting failed\n");
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
                       KEY_LIBRARY_IMAGE_ARRAY, &array) < 0) {
        debug("Unpacking library object failed\n");
        util::print_json_error(error);
        result = false;
    } else  {
        json_array_foreach(array, index, value) {
            if (value) {
                auto* new_image = new image();
                if (!new_image->read_from_config(value, error)) {
                    debug("Error while reading image from library\n");
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

    m_loaded = result;
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



bool image_library::add_image(const char *file_name, std::deque<const tag *> &folder_tags, std::deque<const tag*>& add_tags)
{
    if (!file_name || strlen(file_name) < 1)
        return false;
    char* full_path = nullptr;

    full_path = util::append(full_path, file_name);
    bool result = true;

    if (config::values.check_duplicates) {
        for (const auto& img : m_images) {
            if (strcmp(full_path, img->path()) == 0) {
                debug("Duplicate image: %s\n", full_path);
                result = false;
                break;
            }
        }
    }

    if (result)
        m_images.emplace_back(new image(full_path, tags));

    free((void*) full_path);
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
    bool result = true;

    if (config::values.check_duplicates) {
        for (const auto& img : m_images) {
            if (strcmp(full_path, img->path()) == 0) {
                debug("Duplicate image: %s\n", full_path);
                result = false;
                break;
            }
        }
    }

    if (result)
        m_images.emplace_back(new image(full_path, tags));

    free((void*) full_path);
    return result;
}

const image_vector* image_library::images() const
{
    return &m_images;
}

int image_library::image_count() const
{
    return m_images.size();
}

bool image_library::loaded() const
{
    return m_loaded;
}

namespace library {
    bool read_library(int* return_value) {
        *return_value = config::SUCCESS;
        if (config::values.library->loaded())
            return false; /* nothing to do */

        if (strlen(config::values.library_path) < 1) {
            debug("No image library path provided\n");
            *return_value = config::MISSING_ARG;
            return false;
        }

        json_error_t error;
        json_t* library_obj = nullptr;
        if (util::file_exists(config::values.library_path)) {
            library_obj = json_load_file(config::values.library_path, 0, &error);

            if (!library_obj || !config::values.library->
                    read_from_config(library_obj, &error)) {
                 *return_value = config::READ_LIBRARY_FAILED;
            } else {
                debug("Successfully loaded %i images\n",
                          config::values.library->image_count());
            }
        } else {
            /* Create empty placeholder file */
            library_obj = json_object();
            if (config::values.library->write_to_config(library_obj, &error)) {
                if (json_dump_file(library_obj, config::values.library_path, 0) < 0) {
                    debug("Writing library placeholder json failed\n");
                    *return_value = config::WRITE_EMPTY_LIBRARY_FAILED;
                } else {
                    debug("Successfully created default library\n");
                }
            } else {
                debug("Creating empty library json failed\n");
                *return_value = config::WRITE_EMPTY_LIBRARY_FAILED;
            }
        }

        if (library_obj)
            json_decref(library_obj);
        return false;
    }
}
