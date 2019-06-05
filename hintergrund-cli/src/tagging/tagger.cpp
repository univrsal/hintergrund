/* tagger.cpp created on 2019.6.4
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
#include "tagger.hpp"
#include "../util/config.hpp"
#include <cstring>
#include <unistd.h>

tagger::tagger()
{
    m_tag_counter = 0;
}

bool tagger::write_to_config(json_t *config, json_error_t *error)
{
    bool result = true;
    auto* tag_array = json_array();
    if (tag_array) {
        for (auto& tag : m_tags) {
            if (!tag->write_to_config(config, error)) {
                printf("Error while writing tag\n");
                result = false;
                break;
            }
        }

        if (result && json_object_set_new(config, KEY_TAGGER_TAG_ARRAY, tag_array) < 0) {
            printf("Error while setting tag array\n");
            result = false;
        }
    }

    if (result && json_object_set_new(config, KEY_TAGGER_MAX_DEPTH, json_integer(m_max_depth)) < 0) {
        printf("Error saving tagger max folder depth\n");
        result = false;
    }
    return result;
}

bool tagger::read_from_config(json_t *config, json_error_t *error)
{
    bool result = true;
    auto* tag_array = json_object_get(config, KEY_TAGGER_TAG_ARRAY);

    if (tag_array) {
        size_t index;
        json_t* value;

        json_array_foreach(tag_array, index, value) {
            tag* new_tag = new tag();
            if (!new_tag->read_from_config(value, error)) {
                result = false;
                printf("Error while reading tag from tag array\n");
                delete new_tag;
                break;
            } else {
                if (new_tag->id() > m_tag_counter)
                    m_tag_counter = new_tag->id();
                m_tags.emplace_back(new_tag);
            }
        }
        m_tag_counter++; /* contains the highest id loaded, so now it contains the next available one */
    }
    return result;
}

void tagger::add_new_tag(const char *name, float weight)
{
    bool unique = true;
    for (const auto& tag : m_tags)
    {
        if (strcmp(tag->name(), name) == 0) {
            printf("Duplicate tag name\n");
            unique = false;
            break;
        }
    }

    if (unique) {
        printf("Added new tag %s\n", name);
        m_tags.emplace_back(new tag(name, weight, m_tag_counter++));
    }
}

void tagger::tag_string(const char *str)
{
    if (!str || strlen(str) <= 0)
        return;
    size_t it = 0, old_it = 0, len = strlen(str);

    char temp_tag[256]; /* file/directory names can't be longer than that */

    while (it < len) {
        while (str[it] == ' ') { it++; }
        old_it = it; /* Skip any spaces at the beginning */

        while (str[it] != ' ' && it < len) { it++; } /* Skip until next space or end of string*/

        if (it - old_it < 1) continue; /* Ignore empty tags */

        memcpy(temp_tag, str + old_it, it - old_it);
        temp_tag[it - old_it] = '\0';
        add_new_tag(temp_tag, 1.f);
    }
}

void tagger::iterate_folder(DIR *d, int depth, std::stack<const char*>& current_tags)
{
    if (!d)
        return;
    if (depth >= config::values.max_folder_depth) {
        printf("Folder depth exceeded maximum of %i\n", config::values.max_folder_depth);
        return;
    }
    struct dirent *entry;

    while ((entry = readdir(d))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (entry->d_type == DT_DIR) {

            DIR* temp = opendir(entry->d_name);
            if (temp) {
                tag_string(entry->d_name);
                /* Gets the last added tag and adds it to the temporary stack */
                current_tags.push(m_tags.back()->name());
                chdir(entry->d_name); /* Go into folder */

                iterate_folder(temp, ++depth, current_tags);

                closedir(temp);
                chdir(".."); /* Go back out of folder */
                current_tags.pop(); /* Take previously added tag off of stack since the folder was left */
            } else {
                printf("Error opening \"%s\"\n", entry->d_name);
            }
        } else {
            if (config::values.tag_image_names) {
                /* Separate file name by commas and put results into tags */
                tag_string(entry->d_name);
            }


        }
    }
}

void tagger::auto_tag(const char *root_folder)
{
    char cwd[2049]; /* current working directory */
    if (!getcwd(cwd, 2049)) {
        printf("Error getting current working directory\n");
        return;
    }
    std::stack<const char*> temp_tags;
    chdir(root_folder); /* Start in root folder and then work through folder tree */

    DIR *dp = opendir(root_folder);
    if (!dp) {
        printf("Couldn't open root folder\n");
        return;
    }

    iterate_folder(dp, 0, temp_tags);

    chdir(cwd); /* Revert to original working directory */
}
