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
#include <algorithm>

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

    if (json_unpack_ex(config, error, 0, "{ss}",
                       KEY_IMAGE_NAME, &tmp_path)) {
        debug("Error unpacking tag body\n");
    } else {
        tag_array = json_object_get(config, KEY_IMAGE_TAGS);

        m_name = strdup(tmp_path);
        /* An image object will only have a tag array object
         * if the image was tagged with additional tags
         */
        if (tag_array) {
            size_t index;
            json_t* value;
            const tag* tmp_tag;
            json_array_foreach(tag_array, index, value) {
                tmp_tag = config::values.tag_manager->
                      get_tag_for_str(json_string_value(value));

                if (tmp_tag)
                    m_additional_tags.emplace_back(tmp_tag);
                else
                    util::log("Couldn't find tag \"%s\" for image file \"%s\"\n",
                              json_string_value(value), tmp_tag);
            }
        }
    }

    return result;
}

bool image::write_to_config(json_t *config, json_error_t *error) const
{
    bool result = true;
    json_t* tag_array = nullptr;
    json_t* image_json = json_pack_ex(error, 0, "{ss}",
                      KEY_IMAGE_NAME, m_name);
    if (image_json) {
        if (m_additional_tags.size() > 0) {
            /* Do not write the tag array if it would be empty */
            tag_array = json_array();
            for (const auto& tag : m_additional_tags) {
                if (json_array_append_new(tag_array, json_string(tag->name()))) {
                    debug("Error writing tag \"%s\" to json\n", tag->name());
                    result = false;
                    break;
                }
            }

            if (result && json_object_set_new(image_json, KEY_IMAGE_TAGS, tag_array) < 0) {
                debug("Error writing tag array to json\n");
                result = false;
            }
        }
    } else {
        result = false;
        debug("Error while packing image json\n");
    }

    if (result)
        json_array_append_new(config, image_json);
    return result;
}

void image::remove_custom_tag(const tag *t)
{
    auto tag = std::find(m_additional_tags.begin(), m_additional_tags.end(), t);
    if (tag != m_additional_tags.end())
        m_additional_tags.erase(tag);
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

#ifdef HINTERGRUND_UI
void image::path(QString& str) const
{
    str.clear();
    if (m_place) {
        auto* r = m_place->root();
        if (r && r->path()) {
            str += m_place->root()->path();
            str += "/";
        }
    } else {
        debug("Error: image file has no folder instance\n");
    }

    for (const auto& tag : m_folder_tags) {
        str += tag->name();
        str += "/";
    }
    str += m_name;
}
#endif

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

const std::vector<const tag*>& image::additional_tags() const
{
    return m_additional_tags;
}

const std::vector<const tag*>& image::folder_tags() const
{
    return m_folder_tags;
}

bool image::add_tag(const tag *t)
{
    bool exists = false;
    for (const auto& tag : m_additional_tags) {
        if (tag->id() == t->id()) {
            exists = true;
            break;
        }
    }

    if (!exists) {
        for (const auto& tag : m_folder_tags) {
            if (tag->id() == t->id()) {
                exists = true;
                break;
            }
        }
    }

    if (exists)
        debug("Tried to add existing tag \"%s\" to file \"%s\"\n", t->name(), m_name);
    else
        m_additional_tags.emplace_back(t);
    return !exists;
}
