/* image.hpp created on 2019.6.5
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
#pragma once
#include <vector>
#include <jansson.h>
#include <deque>
#define KEY_IMAGE_NAME  "name"
#define KEY_IMAGE_TAGS  "tags"
#ifdef HINTERGRUND_UI
#include <QString>
#endif
class tag;
class folder;

class image
{
    /* Filename on hdd */
    const char* m_name;
    /* Direct pointer to folder in library::base_folders */
    const folder* m_place;

    std::vector<const tag*> m_additional_tags; /* User defined tags or file name*/
    std::vector<const tag*> m_folder_tags; /* Tags applied over file path (not saved to library) */
public:
    image();
    image(const folder* place, const char* name, const std::deque<const tag*>& path_tags);
    image(const folder* place, const char* name, const std::deque<const tag*>& path_tags, const std::deque<const tag*>& add_tags);
    ~image();

    bool write_to_config(json_t* config, json_error_t* error) const;
    bool read_from_config(json_t* config, json_error_t* error);

    /* Direct pointer */
    const char* name() const;
    /* Constructs a full path based on the folder tags. Needs to be free()'d */
    const char* path() const;
#ifdef HINTERGRUND_UI
    void path(QString& str) const;
#endif
    void set_path_tags(const std::deque<const tag*>& pt);
    void set_place(const folder* f);

    const std::vector<const tag*>& additional_tags() const;
    const std::vector<const tag*>& folder_tags() const;

    bool add_tag(const tag* t);
};
