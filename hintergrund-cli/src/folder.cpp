/* folder.cpp created on 2019.6.15
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
#include "folder.hpp"
#include "util/config.hpp"
#include "util/util.hpp"
#include "tagging/tag.hpp"
#include "tagging/tagger.hpp"
#include "images/image_library.hpp"
#include <unistd.h>
#include <cstring>

folder::folder()
{
    m_path = nullptr;
}

folder::folder(const char* path)
{
    m_path = strdup(path);
    if (path[strlen(path)] == '/')
        m_path[strlen(path)] = '\0';
}

folder::~folder()
{
    free((void*) m_path);
    m_sub_folders.clear();
    m_image_files.clear();
}

bool folder::write_to_config(json_t *cfg, json_error_t *error) const
{
    bool result = true;
    json_t* folder_array = json_array(), *image_array = json_array();
    json_t* folder_obj = json_object();

    if (!folder_array || !image_array || !folder_obj) {
        debug("Creating json structures failed\n");
        return false;
    }

    for (const auto& f : m_sub_folders){
        if (!f->write_to_config(folder_array, error)) {
            result = false;
            break;
        }
    }

    if (result) {
        for (const auto& img : m_image_files) {
            if (!img->write_to_config(image_array, error)) {
                result = false;
                break;
            }
            if (json_array_append_new(image_array, image_array)) {
                result = false;
                debug("Appending to image array failed\n");
                break;
            }
        }
    }

    if (result) {
        folder_obj = json_pack_ex(error, 0, "{sisoso}", KEY_FOLDER_PATH, m_path,
                                  KEY_FOLDER_SUB_FOLDER_ARRAY, folder_array,
                                  KEY_FOLDER_IMAGE_FILE_ARRAY, image_array);
        if (!folder_obj) {
            debug("Packing folder object failed\n");
            util::print_json_error(error);
            result = false;
        } else {
            json_array_append_new(cfg, folder_obj);
        }
    }
    return result;
}

bool folder::read_from_config(json_t *cfg, json_error_t *error)
{
    bool result = true;
    char* tmp_path;
    json_t* folder_array, *image_array;

    if (json_unpack_ex(cfg, error, 0, "{sisoso}",
                       KEY_FOLDER_PATH, &tmp_path,
                       KEY_FOLDER_SUB_FOLDER_ARRAY, &folder_array,
                       KEY_FOLDER_IMAGE_FILE_ARRAY, &image_array) < 0) {
        debug("Unpacking folder object failed\n");
        util::print_json_error(error);
        result = false;
    } else {
        size_t index;
        json_t* value;
        json_array_foreach(folder_array, index, value) {
            folder* new_folder = new folder();
            if (new_folder->read_from_config(value, error)) {
                m_sub_folders.emplace_back(new_folder);
            } else {
                delete new_folder;
                result = false;
                break;
            }
        }

        if (result) {
            json_array_foreach(image_array, index, value) {
                image* new_image = new image();
                if (new_image->read_from_config(value, error)) {
                    m_image_files.emplace_back(new_image);
                } else {
                    util::print_json_error(error);
                    delete new_image;
                    result = false;
                    break;
                }
            }
        }
    }

    return result;
}

void folder::iterate_contents(std::deque<const tag *>& current_tags, DIR* d, int depth)
{
    if (!d) {
        debug("Invalid directory pointer\n");
        return;
    }

    if (depth >= config::values.max_folder_depth) {
        debug("Folder depth exceeds maximum of %i\n", config::values.max_folder_depth);
        return;
    }

    struct dirent *entry;

    while ((entry = readdir(d))) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        if (entry->d_type == DT_DIR) {
            DIR* temp = opendir(entry->d_name);
            if (temp) {
                folder* sf = new folder(entry->d_name);
                /* Extract tags from folder name and adds it to the stack */
                int new_tags = config::values.tag_manager->tag_string(entry->d_name, current_tags);
                chdir(entry->d_name); /* Go into folder */
                sf->iterate_contents(current_tags, temp, ++depth);
                closedir(temp);
                chdir(".."); /* Go back out of folder */
                m_sub_folders.emplace_back(sf);
                for (int i = 0; i < new_tags; i++) /* Remove previously added tags */
                    current_tags.pop_front();
            } else {
                debug("Error opening folder \"%s\"\n", entry->d_name);
            }
        } else if (config::values.library->valid_file_type(entry->d_name)) {
            if (config::values.tag_image_names) {
                int new_tags;
                std::deque<const tag*> file_name_tags;
                new_tags = config::values.tag_manager->tag_string(entry->d_name, current_tags);

                m_image_files.emplace_back(new image(entry->d_name, current_tags, file_name_tags));
                /* Remove any previously added tags from the file name */
                for (int i = 0; i < new_tags; i++)
                    current_tags.pop_back();
            } else {
                m_image_files.emplace_back(new image(entry->d_name, current_tags));
            }
        }
    }
}

folder_list& folder::folders()
{
    return m_sub_folders;
}

image_list& folder::images()
{
    return m_image_files;
}

size_t folder::get_file_count() const
{
    size_t c = m_image_files.size();
    for (const auto& folder : m_sub_folders)
        c += folder->get_file_count();
    return c;
}

void folder::get_files(std::vector<const image *> &imgv)
{
    for (const auto& folder : m_sub_folders)
        folder->get_files(imgv);
    for (const auto& image : m_image_files)
        imgv.emplace_back(image.get());
}
folder* folder::create_from_path(const char *path)
{
    folder* result = nullptr;
    static char cwd[2049]; /* current working directory */

    if (!getcwd(cwd, 2049)) {
        debug("Error getting current working directory\n");
        return result;
    }

    std::deque<const tag*> current_tags;
    chdir(path);
    DIR* dp = opendir(path);
    if (!dp) {
        debug("Couldn't open root folder\n");
        return result;
    }

    result = new folder(path);
    result->iterate_contents(current_tags, dp, 0);
    return result;
}
