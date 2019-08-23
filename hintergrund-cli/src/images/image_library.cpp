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
    json_t* base_folders;
    if (json_unpack_ex(config, error, 0, "{siso}",
                       KEY_LIBRARY_SEQUENTIAL_INDEX, &m_sequential_current,
                       KEY_LIBRARY_BASE_FOLDER_ARRAY, &base_folders) < 0) {
        debug("Unpacking library object failed\n");
        util::print_json_error(error);
    } else {
        json_t* value;
        size_t index;
        std::deque<const tag*> tmp_tags(0);

        json_array_foreach(base_folders, index, value) {
            folder* new_base_folder = new folder();
            if (new_base_folder->read_from_config(value, error, tmp_tags, nullptr)) {
                m_base_folders.emplace_back(new_base_folder);
            } else {
                debug("Reading base folder from library array failed\n");
                result = false;
                delete new_base_folder;
                break;
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

size_t image_library::image_count() const
{
    size_t count = 0;
    for (const auto& folder : m_base_folders) {
        count += folder->get_file_count();
    }
    return count;
}

bool image_library::loaded() const
{
    return m_loaded;
}

void image_library::add_folder(folder *f)
{
    m_base_folders.emplace_back(f);
}

void image_library::get_images(std::vector<const image *> &imgv)
{
    for (const auto& folder : m_base_folders)
        folder->get_files(imgv);
}

const folder_list& image_library::base_folders() const
{
    return m_base_folders;
}

folder_list& image_library::base_folders()
{
    return m_base_folders;
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
