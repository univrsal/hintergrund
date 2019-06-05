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
#include <cstring>
#include <unistd.h>

tagger::tagger()
{
    m_max_depth = 50;
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
                m_tags.emplace_back(new_tag);
            }
        }
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
        m_tags.emplace_back(new tag(name, weight));
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

void tagger::iterate_folder(DIR *d, bool use_filenames, int depth)
{
    if (!d)
        return;
    if (depth >= m_max_depth) {
        printf("Folder depth exceeded maximum of %i\n", m_max_depth);
        return;
    }
    struct dirent *entry;

    while ((entry = readdir(d))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (entry->d_type == DT_DIR) {
            tag_string(entry->d_name);
            DIR* temp = opendir(entry->d_name);
            if (temp) {
                chdir(entry->d_name); /* Go into folder */
                iterate_folder(temp, use_filenames, ++depth);
                closedir(temp);
                chdir(".."); /* Go back out of folder */
            } else {
                printf("Error opening \"%s\"\n", entry->d_name);
            }
        } else if (use_filenames) {
            /* Separate file name by commas and put results into tags */
            tag_string(entry->d_name);
        }
    }
}

void tagger::auto_tag(const char *root_folder, bool use_filenames)
{
    char cwd[2049]; /* current working directory */
    if (!getcwd(cwd, 2049)) {
        printf("Error getting current working directory\n");
        return;
    }
    chdir(root_folder); /* Start in root folder and then work through folder tree */

    DIR *dp = opendir(root_folder);
    if (!dp) {
        printf("Couldn't open root folder\n");
        return;
    }

    iterate_folder(dp, use_filenames, 0);

    chdir(cwd); /* Revert to original working directory */
}

bool tagger::write_to_config(json_t *config, json_error_t *error)
{
    bool result = true;

    json_t* array = json_array();

    for (const auto& tag : m_tags)
    {
        if (!tag.write_to_config(array, error)) {
            result = false;
            break;
        }
    }

    return result;
}
