/* image.cpp created on 2019.6.6
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
#include "image.hpp"
#include "../tagging/tag.hpp"
#include "../util/config.hpp"
#include "../tagging/tagger.hpp"
#include "../util/util.hpp"
#include <cstring>

image::image()
{
    m_path = nullptr;
}

image::image(const char* path, std::deque<const tag*>& tags)
{
    m_path = strdup(path);
    for (const auto& tag : tags)
        m_tags.emplace_back(tag);
}

image::~image()
{
    free((void*)m_path);
    m_path = nullptr;
}

bool image::read_from_config(json_t *config, json_error_t *error)
{
    bool result = true;
    const char* tmp_path = nullptr;
    json_t* tag_array = nullptr;

    if (json_unpack_ex(config, error, 0, "{ssso}",
                       KEY_IMAGE_PATH, &tmp_path,
                       KEY_IMAGE_TAGS, &tag_array) < 0) {
        debug("Error unpacking tag body\n");
    } else {
        m_path = strdup(tmp_path);
        size_t index;
        json_t* value;
        const tag* tmp_tag;
        json_array_foreach(tag_array, index, value) {
            tmp_tag = nullptr;
            if (value) {
                tmp_tag = config::values.tag_manager->
                          get_tag_for_str(json_string_value(value));
                if (tmp_tag)
                    m_tags.emplace_back(tmp_tag);
            }
        }
    }

    return result;
}

bool image::write_to_config(json_t *config, json_error_t *error) const
{
    bool result = true;
    json_t* tag_array = json_array();

    if (tag_array) {
        for (const auto& tag : m_tags) {
            if (json_array_append_new(tag_array, json_string(tag->name()))) {
                debug("Error writing tag \"%s\" to json\n", tag->name());
                result = false;
                break;
            }
        }
        json_t* image_json = json_pack_ex(error, 0, "{ssso}",
                                          KEY_IMAGE_PATH, m_path,
                                          KEY_IMAGE_TAGS, tag_array);
        if (image_json) {
            json_array_append_new(config, image_json);
        } else {
             debug("Error while packing image json\n");
        }
    } else {
         debug("Error creating image tag array\n");
        result = false;
    }

    return result;
}

const char* image::path() const
{
    return m_path;
}
