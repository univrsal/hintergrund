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
#include "../util/util.hpp"
#include "../images/image_library.hpp"
#include <cstring>
#include <unistd.h>

tagger::tagger()
{
    m_tag_counter = 0;
    m_loaded = false;
}

const tag* tagger::add_new_tag(const char *name, float weight)
{
    const tag* result = tag_exists(name);

    if (result) {
        debug("Duplicate tag name \"%s\". Ignoring...\n", name);
    } else {
        tag* new_tag = new tag(name, weight, m_tag_counter++);
        m_tags.emplace_back(new_tag);
        result = new_tag;
        debug("Added new tag %s\n", name); /* Qt's debug call modifies the name string so it's called last */
    }
    return result;
}

const tag* tagger::tag_exists(const char *name) const
{
    const tag* result = nullptr;
    for (const auto& tag : m_tags)
    {
        if (strcmp(tag->name(), name) == 0) {
            /* It's not a new tag, but files under this folder should still get this tag */
            result = tag.get();
            break;
        }
    }
    return result;
}

int tagger::tag_string(const char *str, std::deque<const tag*>& current_tags)
{
    if (!str || strlen(str) <= 0)
        return 0;
    size_t it = 0, old_it = 0, len = strlen(str);
    int new_tags = 0;

    char temp_tag[256]; /* file/directory names can't be longer than that */

    while (it < len) {
        while (str[it] == ' ') { it++; }
        old_it = it; /* Skip any spaces at the beginning */

        while (str[it] != ' ' && it < len) { it++; } /* Skip until next space or end of string*/

        if (it - old_it < 1) continue; /* Ignore empty tags */

        memcpy(temp_tag, str + old_it, it - old_it);
        temp_tag[it - old_it] = '\0';
        auto* new_tag = add_new_tag(temp_tag, 1.f);

        if (new_tag) {
            bool can_add = true;
            for (const auto& tag : current_tags) {
                if (strcmp(tag->name(), new_tag->name()) == 0) {
                    debug("Error duplicate tags within folder hierarchie!\n");
                    can_add = false;
                }
            }

            if (can_add) {
                current_tags.push_front(new_tag);
                new_tags++;
            }
        }
    }
    return new_tags;
}

bool tagger::auto_tag(const char *root_folder)
{
    bool result = true;
    folder* new_folder = folder::create_from_path(root_folder);
    if (new_folder) {
        config::values.library->add_folder(new_folder);
        /* Now write newly indexed library */
        json_error_t error;
        json_t* library_object = json_object();

        result = config::values.library->write_to_config(library_object, &error);
        if (!result || json_dump_file(library_object, config::values.library_path, 0) < 0) {
            debug("Error while writing library to json\n");
        } else {
            /* And now write the tags */
            json_t* tag_array = json_array();
            result = config::values.tag_manager->write_to_config(tag_array, &error);
            if (!result || json_dump_file(tag_array, config::values.tag_path, 0) < 0) {
                debug("Error while writing tag array to json\n");
            }

            json_decref(tag_array);
        }
        json_decref(library_object);
    } else {
        result = false;
    }
    return result;
}

const tag* tagger::get_tag_for_str(const char *string) {
    if (!string)
        return nullptr;

    for (const auto& tag : m_tags) {
        if (strcmp(tag->name(), string) == 0) {
           return tag.get();
        }
    }
    return nullptr;
}

bool tagger::write_to_config(json_t *config, json_error_t *error)
{
    bool result = true;
    for (auto& tag : m_tags) {
        if (!tag->write_to_config(config, error)) {
            debug("Error while writing tag\n");
            result = false;
            break;
        }
    }

    return result;
}

bool tagger::read_from_config(json_t *config, json_error_t *error)
{
    bool result = true;
    m_tags.clear();
    m_tag_counter = 0;
    size_t index;
    json_t* value;

    json_array_foreach(config, index, value) {
        tag* new_tag = new tag();
        if (!new_tag->read_from_config(value, error)) {
            result = false;
            debug("Error while reading tag from tag array\n");
            delete new_tag;
            break;
        } else {
            if (new_tag->id() > m_tag_counter)
                m_tag_counter = new_tag->id();
            m_tags.emplace_back(new_tag);
        }
    }
    m_tag_counter++; /* contains the highest id loaded, so now it contains the next available one */

    m_loaded = result;
    return result;
}

int tagger::tag_count() const
{
    return m_tags.size();
}

bool tagger::loaded() const
{
    return m_loaded;
}

const std::vector<std::unique_ptr<tag>>& tagger::tags() const
{
    return m_tags;
}

void tagger::clear_tags()
{
    m_tags.clear();
}

namespace tagging {
    bool read_tags(int* return_value)
    {
        *return_value = config::SUCCESS;
        if (config::values.tag_manager->loaded())
            return false;

        if (strlen(config::values.tag_path) < 1)
        {
            *return_value = config::READ_TAGS_FAILED;
            debug("No tag file path provided\n");
        } else {
            if (util::file_exists(config::values.tag_path)) {
                json_error_t error;
                json_t* tag_array = json_load_file(config::values.tag_path, 0, &error);

                if (!tag_array || !config::values.tag_manager->read_from_config(tag_array, &error)) {
                    debug("Loading tag array failed\n");
                    util::print_json_error(&error);
                    *return_value = config::READ_TAGS_FAILED;
                } else {
                    debug("Successfully loaded %i tags\n",
                              config::values.tag_manager->tag_count());
                }
            } else {
                /* Create empty placeholder */
                json_t* tag_array = json_array();

                if (!tag_array || json_dump_file(tag_array, config::values.tag_path, 0) < 0) {
                    debug("Writing creating placeholder tags file\n");
                    *return_value = config::READ_TAGS_FAILED;
                } else {
                    debug("Successfully created default tags file\n");
                }

                json_decref(tag_array);
            }
        }
        return false;
    }

    bool tag_path(int* return_value) {
        *return_value = config::SUCCESS;

        if (strlen(config::values.library_path) < 1) {
            debug("Error: No image library path provided\n");
            *return_value = config::MISSING_ARG;
        }

        if (strlen(config::values.auto_tag_path) < 1) {
            debug("Error: No auto tag path provided\n");
            *return_value = config::MISSING_ARG;
        }

        if (*return_value == config::SUCCESS &&
                !config::values.tag_manager->auto_tag(config::values.auto_tag_path)) {
            *return_value = config::AUTO_TAG_FAILED;
        }

        return true; /* True means exit after this operation */
    }
}
