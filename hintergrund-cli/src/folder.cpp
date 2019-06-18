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
}

bool folder::write_to_config(json_t *cfg, json_error_t *error) const
{
    bool result = true;

    return result;
}

bool folder::read_from_config(json_t *cfg, json_error_t *error)
{
    bool result = true;


    return result;
}

void folder::iterate_contents(std::deque<const tag *> current_tags, DIR* d, int depth)
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

        } else if (config::values.library->valid_file_type(entry->d_name)) {
            if (config::values.tag_image_names) {
                int new_tags;
                std::deque<const tag*> file_name_tags;
                new_tags = config::values.tag_manager->tag_string(entry->d_name, current_tags);

                /* Remove any previously added tags from the file name */
                for (int i = 0; i < new_tags; i++)
                    current_tags.pop_back();

            } else {

            }
        }
    }
}

folder* folder::create_from_path(const char *path)
{
    folder* result = nullptr;
    int m_img_counter = 0;
    int m_new_tag_counter = 0;
    char cwd[2049]; /* current working directory */

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
