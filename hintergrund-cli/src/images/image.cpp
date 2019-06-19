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
#include "../folder.hpp"
#include <cstring>
#include <sstream>

image::image()
{
    m_name = nullptr;
}

image::image(const folder* place, const char* path,
             const std::deque<const tag*>& path_tags)
{
    m_place = place;
    m_name = strdup(path);
    for (const auto& tag : path_tags)
        m_folder_tags.emplace_back(tag);
}

image::image(const folder* place,
             const char* path, const std::deque<const tag*>& path_tags, const std::deque<const tag*>& add_tags)
{
    m_place = place;
    m_name = strdup(path);
    for (const auto& tag : path_tags)
        m_folder_tags.emplace_back(tag);
    for (const auto& tag : add_tags)
        m_additional_tags.emplace_back(tag);
}

image::~image()
{
    free((void*)m_name);
    m_name = nullptr;
}

bool image::read_from_config(json_t *config, json_error_t *error)
{
    bool result = true;
    const char* tmp_path = nullptr;
    json_t* tag_array = nullptr;

    if (json_unpack_ex(config, error, 0, "{ssso}",
                       KEY_IMAGE_NAME, &tmp_path,
                       KEY_IMAGE_TAGS, &tag_array) < 0) {
        debug("Error unpacking tag body\n");
    } else {
        m_name = strdup(tmp_path);
        size_t index;
        json_t* value;
        const tag* tmp_tag;
        json_array_foreach(tag_array, index, value) {
            tmp_tag = config::values.tag_manager->
                      get_tag_for_str(json_string_value(value));
            if (tmp_tag)
                m_additional_tags.emplace_back(tmp_tag);
        }
    }

    return result;
}

bool image::write_to_config(json_t *config, json_error_t *error) const
{
    bool result = true;
    json_t* tag_array = json_array();

    if (tag_array) {
        for (const auto& tag : m_additional_tags) {
            if (json_array_append_new(tag_array, json_string(tag->name()))) {
                debug("Error writing tag \"%s\" to json\n", tag->name());
                result = false;
                break;
            }
        }
        json_t* image_json = json_pack_ex(error, 0, "{ssso}",
                                          KEY_IMAGE_NAME, m_name,
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

const char* image::name() const
{
    return m_name;
}

const char* image::path() const
{
    std::stringstream stream;
    if (m_place) {
        auto* r = m_place->root();
        if (r && r->path())
            stream << m_place->root()->path() << "/";
    } else {
        debug("Error: image file has no folder instance\n");
    }

    for (const auto& tag : m_folder_tags)
        stream << tag->name() << "/";
    stream << m_name;
    return strdup(stream.str().c_str());
}

void image::set_path_tags(const std::deque<const tag *> &pt)
{
    m_folder_tags.clear();
    for (const auto& tag : pt)
        m_folder_tags.emplace_back(tag);
}

void image::set_place(const folder *f)
{
    m_place = f;
}
