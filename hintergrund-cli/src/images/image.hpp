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
#define KEY_IMAGE_PATH  "path"
#define KEY_IMAGE_TAGS  "tags"

class tag;

class image
{
    const char* m_path; /* Full file path on hdd */
    std::vector<const tag*> m_additional_tags; /* User defined tags or file name*/
    std::vector<const tag*> m_folder_tags; /* Tags applied over file path (not saved to library) */
public:
    image();
    image(const char* path, const std::deque<const tag*>& path_tags);
    image(const char* path, const std::deque<const tag*>& path_tags, const std::deque<const tag*>& add_tags);
    ~image();

    bool write_to_config(json_t* config, json_error_t* error) const;
    bool read_from_config(json_t* config, json_error_t* error);

    const char* path() const;
};
